import numpy as np
import ROOT
from root_numpy import fill_hist

def add_new_variables(event):
    # hardcoded...
    muon_mass = 105.6583745/1000.
    add_info = np.full(event['Muons_Minv_MuMu_Fsr_nearOnly'].shape, -1.,
                       dtype=[
                           ('Z_PT_FSR_nearOnly', np.float32),
                           ('Z_PT_FSR_LOG_nearOnly', np.float32)
                       ])
    has_fsr = event['Muons_Minv_MuMu'] != event['Muons_Minv_MuMu_Fsr_nearOnly']
    add_info['Z_PT_FSR_nearOnly'] = event['Z_PT']

    pt1 = event['Muons_PT_Lead']
    phi1 = event['Muons_Phi_Lead']
    pt2 = event['Muons_PT_Sub']
    phi2 = event['Muons_Phi_Sub']
    pt3 = event['FSR_Et']
    phi3 = event['FSR_Phi']
    ## Z PT with FSR
    z_pt_fsr = np.sqrt((pt1*np.cos(phi1) + pt2*np.cos(phi2) * pt3*np.cos(phi3))**2 \
                       + (pt1*np.sin(phi1) + pt2*np.sin(phi2) + pt3*np.sin(phi3))**2)
    add_info['Z_PT_FSR_nearOnly'][has_fsr] = z_pt_fsr[has_fsr]

    add_info['Z_PT_FSR_LOG_nearOnly'] = np.log10(add_info['Z_PT_FSR_nearOnly'])
    return add_info


def merge_arrays(arr1, arr2):
    t1 = arr1.dtype
    t2 = arr2.dtype
    newdtype = np.dtype(t1.descr + t2.descr)
    newarray = np.empty(shape=arr1.shape, dtype=newdtype)
    for field in t1.names:
        newarray[field] = arr1[field]
    for field in t2.names:
        newarray[field] = arr2[field]
    return newarray


def get_Zpt_weights(file_name, hist_base_name, verbose=False):
    """Zpt reweighting is done for each jet bin [0, 1, >=2]"""
    weights_dict = {}

    re_file = ROOT.TFile.Open(file_name)
    for jet_idx in range(3):
        hist_name = '{}_{}JetMCRatio2'.format(hist_base_name, jet_idx)
        print(hist_name)
        hist = re_file.Get(hist_name)
        # x_bins = [x for x in hist.GetXaxis().GetXbins()]
        weight_list = []
        for ibin in range(1, hist.GetNbinsX()+1):
            low = hist.GetBinLowEdge(ibin)
            high = hist.GetBinLowEdge(ibin+1)
            content = hist.GetBinContent(ibin)
            if verbose:
                print("[{:.3f}, {:.3f}]: {:.3f}".format(low, high, content))
            weight_list.append((low, high, content))

        weights_dict[jet_idx] = weight_list

    re_file.Close()
    return weights_dict


def make_hist(event, hist_defs, post_fix=None, weights=None, mask=None):
    histograms = []

    for hist_opt in hist_defs:
        if(len(hist_opt) < 6):
            print(hist_opt, "not enough info")
            continue

        h_type, branch_name, h_low, h_high, h_nbins = hist_opt[:5]

        # histogram name
        if post_fix is not None:
            h_name = "{}_{}".format(branch_name, post_fix)
        else:
            h_name = branch_name

        if len(hist_opt) > 6:
            try:
                h_xlabel, h_ylabel = hist_opt[5:7]
            except ValueError:
                print(h_name)
                raise

        else:
            h_xlabel, h_ylabel = None, None

        if 'TH1' in h_type:
            h1 = getattr(ROOT, h_type)(h_name, h_name, h_nbins, h_low, h_high)
            if h_xlabel:
                h1.SetXTitle(h_xlabel)
                h1.SetYTitle(h_ylabel)
        else:
            print("Not implemented")
            continue

        if weights is not None:
            if mask is None:
                fill_hist(h1, event[branch_name], weights=weights)
            else:
                fill_hist(h1, event[branch_name][mask], weights=weights[mask])
        else:
            if mask is None:
                fill_hist(h1, event[branch_name])
            else:
                fill_hist(h1, event[branch_name][mask])

        histograms.append(h1)

    return histograms
