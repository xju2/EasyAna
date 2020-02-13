## Introduction
Fast Data Analysis on HPC

## Installation

Write following to `~/.condarc`, so new environments are in this directory.

```
envs_dirs:
  - ~/.conda/envs
report_errors: true
```

Create a conda env
```bash
module load python
conda create --name FastAna python=3.7
source activate FastAna
```

Install [mpi4py](https://docs.nersc.gov/programming/high-level-environments/python/mpi4py/)
```
wget https://bitbucket.org/mpi4py/mpi4py/downloads/mpi4py-3.0.0.tar.gz
tar zxvf mpi4py-3.0.0.tar.gz
cd mpi4py-3.0.0

module unload craype-hugepages2M
```
Now check `which cc` actually returns something like: `/opt/cray/pe/craype/2.5.18/bin/cc`
If yes, proceed:
```
python setup.py build --mpicc="$(which cc) -shared"
python setup.py install
```

Then install ROOT
```
conda install -c conda-forge root cmake
```

Then install FastAna in a *new* directory
```bash
git clone https://gitlab.cern.ch/berkeleylab/fastanalysis.git FastAnalysis
cd FastAnalysis
pip install -e .
```


## Run
Input files are shared at: `/global/project/projectdirs/atlas/xju/examples/H5vsROOT`

Python, no MPI
```
make_Hmumu_hist make_hist_sherpa.yaml reduced_merged_3jet_6_withBDT.root 0 -1
```

Python, with MPI. First test MPI
```
srun -n 4 test_mpi
```

Then run on the files:
```
srun -n 8 make_Hmumu_hist_mpi make_hist_sherpa.yaml reduced_merged_3jet_6_withBDT.root test.root
```

Then run on the H5 file:
```
srun -n 8 make_Hmumu_hist_h5 make_hist_sherpa.yaml merged_3jet_6.h5 test_H5_input.root
```

## Ray Hello World

Install ray in the conda environment through pip:
```
conda install pip
pip install "ray[debug]"
```

Run the test script in an interactive session:
```
salloc -N 2 -C knl -q interactive -t 01:00:00 -A atlas
source construct_ray_cluster 32
test_ray
```
## Distributed data processing with Ray

Define tasks to process (split input files into equally sized chunks):
```
define_tasks -i /global/project/projectdirs/atlas/xju/examples/H5vsROOT/ reduced_merged_3jet_6_withBDT.root -t DiMuonNtuple
```

Use 'raydistribute' script to distribute the tasks across multiple nodes.

### Run tasks in parallel on one node

Use the '-l' option to run locally and use the '--dry-run' option to only print out the commands:
```
raydistribute make_Hmumu_hist config/make_hist_sherpa.yaml -l --dry-run
```

If commands look good, execute them by removing the --dry-run option.

### Run tasks across many nodes

Allocate some nodes as in the Hello World example:
```
salloc -N 2 -C knl -q interactive -t 01:00:00 -A atlas
```

Setup Ray and construct the Ray cluster
```
source construct_ray_cluster 32
```

First do the 'dry-run':
```
raydistribute make_Hmumu_hist config/make_hist_sherpa.yaml --dry-run
```

If commands look good, execute them by removing the --dry-run option.

### Ray timeline

Add option '--timeline' to the raydistribute process to get a json file with a timeline
automatically created by Ray. The timeline file can be opened with chrome://tracing as shown here:
https://ray.readthedocs.io/en/latest/troubleshooting.html

=======
### Run ROOT
```
mkdir build && cd build
cmake ..
make
```

Run with `TTreeReader`
```
./src/runTTreeReader /global/project/projectdirs/atlas/xju/examples/H5vsROOT/reduced_merged_3jet_6_withBDT.root test.root
```

Run with `TTreeProcessorMT`
```
./src/runTTreeMT /global/project/projectdirs/atlas/xju/examples/H5vsROOT/reduced_merged_3jet_6_withBDT.root test.root
```

Run with `RDataFrame`
```
./src/runRDataFrame /global/project/projectdirs/atlas/xju/examples/H5vsROOT/reduced_merged_3jet_6_withBDT.root test.root
```
