#include <iostream>

#include <TFile.h>
#include <TH1F.h>
#include <ROOT/RDataFrame.hxx>

int main(int argc, char* argv[])
{
  //
  // Parse input options
  if(argc!=3 && argc!=4)
    {
      std::cerr << "usage: " << argv[0] << " file_name out_name [threads]" << std::endl;
      return 1;
    }

  std::string file_name=argv[1];
  std::string out_name =argv[2];
  if(argc==4)
    ROOT::EnableImplicitMT(std::stoi(argv[3]));

  //
  // Load reweighting matrix
  TFile *fh_rwgt=TFile::Open("/global/project/projectdirs/atlas/xju/examples/H5vsROOT/weights.root");
  TH1F *h_rwgt_0=fh_rwgt->Get<TH1F>("Z_PT_FSR_LOG_nearOnly_0JetMCRatio2");
  TH1F *h_rwgt_1=fh_rwgt->Get<TH1F>("Z_PT_FSR_LOG_nearOnly_1JetMCRatio2");
  TH1F *h_rwgt_2=fh_rwgt->Get<TH1F>("Z_PT_FSR_LOG_nearOnly_2JetMCRatio2");

  //
  // Prepare the dataframe
  ROOT::RDataFrame df_root("DiMuonNtuple", file_name);

  // Define the flow
  ROOT::RDF::RNode df_selected=df_root
    .Filter([](float mass)->bool { return mass>110; }, {"Muons_Minv_MuMu_Fsr_nearOnly"} )
    .Filter("Muons_PT_Lead>27")
    .Filter("Muons_PT_Sub>15");

  ROOT::RDF::RNode df_addvar=df_selected
    .Define("hasFSR", "Muons_Minv_MuMu!=Muons_Minv_MuMu_Fsr_nearOnly")
    .Define("Z_PT_FSR_nearOnly", [](bool hasFSR, float Muons_PT_Lead, float Muons_Phi_Lead, float Muons_PT_Sub, float Muons_Phi_Sub, float FSR_Et, float FSR_Phi, float Z_PT)->float
	    { if(hasFSR)
		return sqrtf(
			     powf(Muons_PT_Lead*cosf(Muons_Phi_Lead) + Muons_PT_Sub*cosf(Muons_Phi_Sub) * FSR_Et*cosf(FSR_Phi),2) +
			     powf(Muons_PT_Lead*sinf(Muons_Phi_Lead) + Muons_PT_Sub*sinf(Muons_Phi_Sub) + FSR_Et*sinf(FSR_Phi),2)
			     );
	      else
		return Z_PT;
	    }, { "hasFSR", "Muons_PT_Lead", "Muons_Phi_Lead", "Muons_PT_Sub", "Muons_Phi_Sub", "FSR_Et", "FSR_Phi", "Z_PT" })
    .Define("Z_PT_FSR_LOG_nearOnly", "log10f(Z_PT_FSR_nearOnly)");

  ROOT::RDF::RNode df_rename=df_addvar
    .Define("ClassOut_XGB_Higgs_Norm","ClassOut_XGB_Ht_Higgs")
    .Define("ClassOut_XGB_VBF_Norm"  ,"ClassOut_XGB_Ht_VBF");

  ROOT::RDF::RNode df_reweight=df_rename
    .Define("final_weight", [&](float weight, int Jets_jetMultip, float Z_PT_FSR_LOG_nearOnly)->float
	    {
	      switch(Jets_jetMultip)
		{
		case 0:
		  return weight/h_rwgt_0->GetBinContent(h_rwgt_0->FindBin(Z_PT_FSR_LOG_nearOnly));
		  break;
		case 1:
		  return weight/h_rwgt_1->GetBinContent(h_rwgt_1->FindBin(Z_PT_FSR_LOG_nearOnly));
		  break;
		default:
		  return weight/h_rwgt_2->GetBinContent(h_rwgt_2->FindBin(Z_PT_FSR_LOG_nearOnly));
		  break;
		}
	    }, {"weight","Jets_jetMultip","Z_PT_FSR_LOG_nearOnly"});

  //
  // Save the output histograms
  TFile *fh_out=TFile::Open(out_name.c_str(), "RECREATE");
  df_reweight.Histo1D({"ClassOut_XGB_Higgs_Norm"     ,"",200,0,1     }, "ClassOut_XGB_Higgs_Norm"     , "final_weight")->Write();
  df_reweight.Histo1D({"ClassOut_XGB_VBF_Norm"       ,"",200,0,1     }, "ClassOut_XGB_VBF_Norm"       , "final_weight")->Write();
  df_reweight.Histo1D({"Event_MET"                   ,"",200,0,200   }, "Event_MET"                   , "final_weight")->Write();
  df_reweight.Histo1D({"Muons_Eta_Lead"              ,"",100,-3.0,3  }, "Muons_Eta_Lead"              , "final_weight")->Write();
  df_reweight.Histo1D({"Muons_Phi_Lead"              ,"",64,-3.2,3.2 }, "Muons_Phi_Lead"              , "final_weight")->Write();
  df_reweight.Histo1D({"Muons_PT_Lead"               ,"",200,0,400   }, "Muons_PT_Lead"               , "final_weight")->Write();
  df_reweight.Histo1D({"Muons_Eta_Sub"               ,"",100,-3.0,3  }, "Muons_Eta_Sub"               , "final_weight")->Write();
  df_reweight.Histo1D({"Muons_Phi_Sub"               ,"",64,-3.2,3.2 }, "Muons_Phi_Sub"               , "final_weight")->Write();
  df_reweight.Histo1D({"Muons_PT_Sub"                ,"",200,0,400   }, "Muons_PT_Sub"                , "final_weight")->Write();
  df_reweight.Histo1D({"Muons_Charge_Lead"           ,"",3,-1.5,1.5  }, "Muons_Charge_Lead"           , "final_weight")->Write();
  df_reweight.Histo1D({"Muons_CosThetaStar"          ,"",200,-1.0,1  }, "Muons_CosThetaStar"          , "final_weight")->Write();
  df_reweight.Histo1D({"Muons_Minv_MuMu"             ,"",3000,0,300  }, "Muons_Minv_MuMu"             , "final_weight")->Write();
  df_reweight.Histo1D({"Muons_Minv_MuMu_Fsr"         ,"",3000,0,300  }, "Muons_Minv_MuMu_Fsr"         , "final_weight")->Write();
  df_reweight.Histo1D({"Muons_Minv_MuMu_Fsr_nearOnly","",3000,0,300  }, "Muons_Minv_MuMu_Fsr_nearOnly", "final_weight")->Write();
  df_reweight.Histo1D({"Z_PT_FSR_nearOnly"           ,"",200,0,400   }, "Z_PT_FSR_nearOnly"           , "final_weight")->Write();
  df_reweight.Histo1D({"Z_PT_FSR_LOG_nearOnly"       ,"",400,-3.0,6  }, "Z_PT_FSR_LOG_nearOnly"       , "final_weight")->Write();
  df_reweight.Histo1D({"DiJet_Minv"                  ,"",1000,0,1000 }, "DiJet_Minv"                  , "final_weight")->Write();
  df_reweight.Histo1D({"DiJet_PT"                    ,"",400,0,400   }, "DiJet_PT"                    , "final_weight")->Write();
  df_reweight.Histo1D({"Jets_PT_Lead"                ,"",400,0,400   }, "Jets_PT_Lead"                , "final_weight")->Write();
  df_reweight.Histo1D({"Jets_Eta_Lead"               ,"",100,-5.0,5  }, "Jets_Eta_Lead"               , "final_weight")->Write();
  df_reweight.Histo1D({"Jets_PT_Sub"                 ,"",400,0,400   }, "Jets_PT_Sub"                 , "final_weight")->Write();
  df_reweight.Histo1D({"Jets_Eta_Sub"                ,"",100,-5.0,5  }, "Jets_Eta_Sub"                , "final_weight")->Write();
  df_reweight.Histo1D({"DiJet_Rapidity"              ,"",100,-6,6    }, "DiJet_Rapidity"              , "final_weight")->Write();
  df_reweight.Histo1D({"Jets_Phi_Lead"               ,"",64,-3.2,3.2 }, "Jets_Phi_Lead"               , "final_weight")->Write();
  df_reweight.Histo1D({"Jets_Phi_Sub"                ,"",64,-3.2,3.2 }, "Jets_Phi_Sub"                , "final_weight")->Write();
  df_reweight.Histo1D({"Dphi_JZ_Lead"                ,"",100,-3.2,3.2}, "Dphi_JZ_Lead"                , "final_weight")->Write();
  df_reweight.Histo1D({"Dphi_JZ_Sub"                 ,"",100,-3.2,3.2}, "Dphi_JZ_Sub"                 , "final_weight")->Write();
  df_reweight.Histo1D({"Z_Rapidity"                  ,"",100,-6.0,6  }, "Z_Rapidity"                  , "final_weight")->Write();
  df_reweight.Histo1D({"Jets_jetMultip"              ,"",11,-0.5,10.5}, "Jets_jetMultip"              , "final_weight")->Write();
  fh_out->Close();

  fh_rwgt->Close();
  
  return 0;
}
