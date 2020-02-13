#include <iostream>

#include <TFile.h>
#include <TH1F.h>
#include <TTreeReader.h>
#include <ROOT/TTreeProcessorMT.hxx>
#include <ROOT/TThreadedObject.hxx>

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
  // Prepare the histograms
  ROOT::TThreadedObject<TH1F> h_ClassOut_XGB_Higgs_Norm     ("ClassOut_XGB_Higgs_Norm"     ,"",200,0,1);
  ROOT::TThreadedObject<TH1F> h_ClassOut_XGB_VBF_Norm       ("ClassOut_XGB_VBF_Norm"       ,"",200,0,1);
  ROOT::TThreadedObject<TH1F> h_Event_MET                   ("Event_MET"                   ,"",200,0,200);
  ROOT::TThreadedObject<TH1F> h_Muons_Eta_Lead              ("Muons_Eta_Lead"              ,"",100,-3.0,3);
  ROOT::TThreadedObject<TH1F> h_Muons_Phi_Lead              ("Muons_Phi_Lead"              ,"",64,-3.2,3.2);
  ROOT::TThreadedObject<TH1F> h_Muons_PT_Lead               ("Muons_PT_Lead"               ,"",200,0,400);
  ROOT::TThreadedObject<TH1F> h_Muons_Eta_Sub               ("Muons_Eta_Sub"               ,"",100,-3.0,3);
  ROOT::TThreadedObject<TH1F> h_Muons_Phi_Sub               ("Muons_Phi_Sub"               ,"",64,-3.2,3.2);
  ROOT::TThreadedObject<TH1F> h_Muons_PT_Sub                ("Muons_PT_Sub"                ,"",200,0,400);
  ROOT::TThreadedObject<TH1F> h_Muons_Charge_Lead           ("Muons_Charge_Lead"           ,"",3,-1.5,1.5);
  ROOT::TThreadedObject<TH1F> h_Muons_CosThetaStar          ("Muons_CosThetaStar"          ,"",200,-1.0,1);
  ROOT::TThreadedObject<TH1F> h_Muons_Minv_MuMu             ("Muons_Minv_MuMu"             ,"",3000,0,300);
  ROOT::TThreadedObject<TH1F> h_Muons_Minv_MuMu_Fsr         ("Muons_Minv_MuMu_Fsr"         ,"",3000,0,300);
  ROOT::TThreadedObject<TH1F> h_Muons_Minv_MuMu_Fsr_nearOnly("Muons_Minv_MuMu_Fsr_nearOnly","",3000,0,300);
  ROOT::TThreadedObject<TH1F> h_Z_PT_FSR_nearOnly           ("Z_PT_FSR_nearOnly"           ,"",200,0,400);
  ROOT::TThreadedObject<TH1F> h_Z_PT_FSR_LOG_nearOnly       ("Z_PT_FSR_LOG_nearOnly"       ,"",400,-3.0,6);
  ROOT::TThreadedObject<TH1F> h_DiJet_Minv                  ("DiJet_Minv"                  ,"",1000,0,1000);
  ROOT::TThreadedObject<TH1F> h_DiJet_PT                    ("DiJet_PT"                    ,"",400,0,400);
  ROOT::TThreadedObject<TH1F> h_Jets_PT_Lead                ("Jets_PT_Lead"                ,"",400,0,400);
  ROOT::TThreadedObject<TH1F> h_Jets_Eta_Lead               ("Jets_Eta_Lead"               ,"",100,-5.0,5);
  ROOT::TThreadedObject<TH1F> h_Jets_PT_Sub                 ("Jets_PT_Sub"                 ,"",400,0,400);
  ROOT::TThreadedObject<TH1F> h_Jets_Eta_Sub                ("Jets_Eta_Sub"                ,"",100,-5.0,5);
  ROOT::TThreadedObject<TH1F> h_DiJet_Rapidity              ("DiJet_Rapidity"              ,"",100,-6,6);
  ROOT::TThreadedObject<TH1F> h_Jets_Phi_Lead               ("Jets_Phi_Lead"               ,"",64,-3.2,3.2);
  ROOT::TThreadedObject<TH1F> h_Jets_Phi_Sub                ("Jets_Phi_Sub"                ,"",64,-3.2,3.2);
  ROOT::TThreadedObject<TH1F> h_Dphi_JZ_Lead                ("Dphi_JZ_Lead"                ,"",100,-3.2,3.2);
  ROOT::TThreadedObject<TH1F> h_Dphi_JZ_Sub                 ("Dphi_JZ_Sub"                 ,"",100,-3.2,3.2);
  ROOT::TThreadedObject<TH1F> h_Z_Rapidity                  ("Z_Rapidity"                  ,"",100,-6.0,6);
  ROOT::TThreadedObject<TH1F> h_Jets_jetMultip              ("Jets_jetMultip"              ,"",11,-0.5,10.5);

  //
  // Prepare the processor
  ROOT::TTreeProcessorMT processor(file_name, "DiMuonNtuple");

  processor.Process([&](TTreeReader& reader)
		    {
		      // Select branches to read
		      TTreeReaderValue<float> weight                      (reader, "weight"                      );

		      TTreeReaderValue<float> Event_MET                   (reader, "Event_MET");
		      
		      TTreeReaderValue<float> mass                        (reader, "Muons_Minv_MuMu_Fsr_nearOnly"); // mass_name

		      TTreeReaderValue<float> Muons_PT_Lead               (reader, "Muons_PT_Lead"               );
		      TTreeReaderValue<float> Muons_Eta_Lead              (reader, "Muons_Eta_Lead"              );		      
		      TTreeReaderValue<float> Muons_Phi_Lead              (reader, "Muons_Phi_Lead"              );
		      TTreeReaderValue<short> Muons_Charge_Lead           (reader, "Muons_Charge_Lead"           );

		      TTreeReaderValue<float> Muons_PT_Sub                (reader, "Muons_PT_Sub"                );
		      TTreeReaderValue<float> Muons_Eta_Sub               (reader, "Muons_Eta_Sub"               );
		      TTreeReaderValue<float> Muons_Phi_Sub               (reader, "Muons_Phi_Sub"               );

		      TTreeReaderValue<float> Muons_CosThetaStar          (reader, "Muons_CosThetaStar"          );
		      TTreeReaderValue<float> Muons_Minv_MuMu             (reader, "Muons_Minv_MuMu"             );
		      TTreeReaderValue<float> Muons_Minv_MuMu_Fsr         (reader, "Muons_Minv_MuMu_Fsr"         );
		      TTreeReaderValue<float> Muons_Minv_MuMu_Fsr_nearOnly(reader, "Muons_Minv_MuMu_Fsr_nearOnly");		      

		      TTreeReaderValue<float> FSR_Et                      (reader, "FSR_Et"                      );
		      TTreeReaderValue<float> FSR_Phi                     (reader, "FSR_Phi"                     );

		      TTreeReaderValue<float> Z_PT                        (reader, "Z_PT"                        );

		      TTreeReaderValue<float> ClassOut_XGB_Higgs_Norm     (reader, "ClassOut_XGB_Ht_Higgs"       );
		      TTreeReaderValue<float> ClassOut_XGB_VBF_Norm       (reader, "ClassOut_XGB_Ht_VBF"         );

		      TTreeReaderValue<float> Jets_PT_Lead                (reader, "Jets_PT_Lead"                );
		      TTreeReaderValue<float> Jets_Eta_Lead               (reader, "Jets_Eta_Lead"               );
		      TTreeReaderValue<float> Jets_Phi_Lead               (reader, "Jets_Phi_Lead"               );

		      TTreeReaderValue<float> Jets_PT_Sub                 (reader, "Jets_PT_Sub"                 );
		      TTreeReaderValue<float> Jets_Eta_Sub                (reader, "Jets_Eta_Sub"                );
		      TTreeReaderValue<float> Jets_Phi_Sub                (reader, "Jets_Phi_Sub"                );

		      TTreeReaderValue<int  > Jets_jetMultip              (reader, "Jets_jetMultip"              );

		      TTreeReaderValue<float> DiJet_Minv                  (reader, "DiJet_Minv"                  );
		      TTreeReaderValue<float> DiJet_PT                    (reader, "DiJet_PT"                    );
		      TTreeReaderValue<float> DiJet_Rapidity              (reader, "DiJet_Rapidity"              );

		      TTreeReaderValue<float> Dphi_JZ_Lead                (reader, "Dphi_JZ_Lead"                );
		      TTreeReaderValue<float> Dphi_JZ_Sub                 (reader, "Dphi_JZ_Sub"                 );
		      TTreeReaderValue<float> Z_Rapidity                  (reader, "Z_Rapidity"                  );

		      // Loop the loop
		      while (reader.Next())
			{
			  // Event selection
			  if((*mass         )<110) continue;
			  if((*Muons_PT_Lead)< 27) continue;
			  if((*Muons_PT_Sub )< 15) continue;

			  // Add new variables
			  float z_pt_fsr = sqrtf(
						 powf((*Muons_PT_Lead)*cosf((*Muons_Phi_Lead)) + (*Muons_PT_Sub)*cosf((*Muons_Phi_Sub)) * (*FSR_Et)*cosf((*FSR_Phi)),2) +
						 powf((*Muons_PT_Lead)*sinf((*Muons_Phi_Lead)) + (*Muons_PT_Sub)*sinf((*Muons_Phi_Sub)) + (*FSR_Et)*sinf((*FSR_Phi)),2)
						 );
			  float Z_PT_FSR_nearOnly=((*Muons_Minv_MuMu) == (*Muons_Minv_MuMu_Fsr_nearOnly))?(*Z_PT):z_pt_fsr;

			  float Z_PT_FSR_LOG_nearOnly = log10f(Z_PT_FSR_nearOnly);

			  // Calculate weight
			  float final_weight=(*weight);
			  switch(*Jets_jetMultip)
			    {
			    case 0:
			      final_weight/=h_rwgt_0->GetBinContent(h_rwgt_0->FindBin(Z_PT_FSR_LOG_nearOnly));
			      break;
			    case 1:
			      final_weight/=h_rwgt_1->GetBinContent(h_rwgt_1->FindBin(Z_PT_FSR_LOG_nearOnly));
			      break;
			    default:
			      final_weight/=h_rwgt_2->GetBinContent(h_rwgt_2->FindBin(Z_PT_FSR_LOG_nearOnly));
			      break;
			    }

			  // Fill histograms
			  h_ClassOut_XGB_Higgs_Norm     ->Fill(*ClassOut_XGB_Higgs_Norm     ,final_weight);
			  h_ClassOut_XGB_VBF_Norm       ->Fill(*ClassOut_XGB_VBF_Norm       ,final_weight);
			  h_Event_MET                   ->Fill(*Event_MET                   ,final_weight);
			  h_Muons_Eta_Lead              ->Fill(*Muons_Eta_Lead              ,final_weight);
			  h_Muons_Phi_Lead              ->Fill(*Muons_Phi_Lead              ,final_weight);
			  h_Muons_PT_Lead               ->Fill(*Muons_PT_Lead               ,final_weight);
			  h_Muons_Eta_Sub               ->Fill(*Muons_Eta_Sub               ,final_weight);
			  h_Muons_Phi_Sub               ->Fill(*Muons_Phi_Sub               ,final_weight);
			  h_Muons_PT_Sub                ->Fill(*Muons_PT_Sub                ,final_weight);
			  h_Muons_Charge_Lead           ->Fill(*Muons_Charge_Lead           ,final_weight);
			  h_Muons_CosThetaStar          ->Fill(*Muons_CosThetaStar          ,final_weight);
			  h_Muons_Minv_MuMu             ->Fill(*Muons_Minv_MuMu             ,final_weight);
			  h_Muons_Minv_MuMu_Fsr         ->Fill(*Muons_Minv_MuMu_Fsr         ,final_weight);
			  h_Muons_Minv_MuMu_Fsr_nearOnly->Fill(*Muons_Minv_MuMu_Fsr_nearOnly,final_weight);
			  h_Z_PT_FSR_nearOnly           ->Fill( Z_PT_FSR_nearOnly           ,final_weight);
			  h_Z_PT_FSR_LOG_nearOnly       ->Fill( Z_PT_FSR_LOG_nearOnly       ,final_weight);
			  h_DiJet_Minv                  ->Fill(*DiJet_Minv                  ,final_weight);
			  h_DiJet_PT                    ->Fill(*DiJet_PT                    ,final_weight);
			  h_Jets_PT_Lead                ->Fill(*Jets_PT_Lead                ,final_weight);
			  h_Jets_Eta_Lead               ->Fill(*Jets_Eta_Lead               ,final_weight);
			  h_Jets_PT_Sub                 ->Fill(*Jets_PT_Sub                 ,final_weight);
			  h_Jets_Eta_Sub                ->Fill(*Jets_Eta_Sub                ,final_weight);
			  h_DiJet_Rapidity              ->Fill(*DiJet_Rapidity              ,final_weight);
			  h_Jets_Phi_Lead               ->Fill(*Jets_Phi_Lead               ,final_weight);
			  h_Jets_Phi_Sub                ->Fill(*Jets_Phi_Sub                ,final_weight);
			  h_Dphi_JZ_Lead                ->Fill(*Dphi_JZ_Lead                ,final_weight);
			  h_Dphi_JZ_Sub                 ->Fill(*Dphi_JZ_Sub                 ,final_weight);
			  h_Z_Rapidity                  ->Fill(*Z_Rapidity                  ,final_weight);
			  h_Jets_jetMultip              ->Fill(*Jets_jetMultip              ,final_weight);
			}
		    }
		    );

  //
  // Save the output histograms
  TFile *fh_out=TFile::Open(out_name.c_str(), "RECREATE");
  h_ClassOut_XGB_Higgs_Norm     .Merge()->Write();
  h_ClassOut_XGB_VBF_Norm       .Merge()->Write();
  h_Event_MET                   .Merge()->Write();
  h_Muons_Eta_Lead              .Merge()->Write();
  h_Muons_Phi_Lead              .Merge()->Write();
  h_Muons_PT_Lead               .Merge()->Write();
  h_Muons_Eta_Sub               .Merge()->Write();
  h_Muons_Phi_Sub               .Merge()->Write();
  h_Muons_PT_Sub                .Merge()->Write();
  h_Muons_Charge_Lead           .Merge()->Write();
  h_Muons_CosThetaStar          .Merge()->Write();
  h_Muons_Minv_MuMu             .Merge()->Write();
  h_Muons_Minv_MuMu_Fsr         .Merge()->Write();
  h_Muons_Minv_MuMu_Fsr_nearOnly.Merge()->Write();
  h_Z_PT_FSR_nearOnly           .Merge()->Write();
  h_Z_PT_FSR_LOG_nearOnly       .Merge()->Write();
  h_DiJet_Minv                  .Merge()->Write();
  h_DiJet_PT                    .Merge()->Write();
  h_Jets_PT_Lead                .Merge()->Write();
  h_Jets_Eta_Lead               .Merge()->Write();
  h_Jets_PT_Sub                 .Merge()->Write();
  h_Jets_Eta_Sub                .Merge()->Write();
  h_DiJet_Rapidity              .Merge()->Write();
  h_Jets_Phi_Lead               .Merge()->Write();
  h_Jets_Phi_Sub                .Merge()->Write();
  h_Dphi_JZ_Lead                .Merge()->Write();
  h_Dphi_JZ_Sub                 .Merge()->Write();
  h_Z_Rapidity                  .Merge()->Write();
  h_Jets_jetMultip              .Merge()->Write();
  fh_out->Close();

  fh_rwgt->Close();
  
  return 0;
}
