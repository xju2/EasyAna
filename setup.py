from __future__ import absolute_import
from __future__ import division
from __future__ import print_function


from setuptools import find_packages
from setuptools import setup

description="Perform High Energy Physics with Numpy"

setup(
    name='FastAnalysis',
    version='0.1.0',
    description=description,
    long_description=description,
    author='Berkeley Lab',
    keywords=["numpy", "HEP", "analysis", "root_numpy", "ROOT"],
    url="https://gitlab.cern.ch/berkeleylab/fastanalysis",
    packages=find_packages(),
    install_requires=[
        "setuptools",
        'root_numpy',
        'pyyaml>=5.1',
        'h5py',
    ],
    setup_requires=[],
    classifiers=[
        "Programming Language :: Python :: 3.7",
    ],
    scripts=[
        'scripts/raydistribute',
        'scripts/define_tasks',
        'scripts/ray_sync',
        'scripts/construct_ray_cluster',
        'scripts/test_ray',
        'scripts/test_mpi',
        'scripts/make_Hmumu_hist',
        'scripts/make_Hmumu_hist_mpi',
        'scripts/make_Hmumu_hist_h5',
    ],
)
