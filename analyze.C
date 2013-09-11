#include <iostream>
#include <fstream>
#include <fstream>
#include <sstream>
#include <string>

#include "TH1.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TString.h"

double get_max_y(TH1D* hist[4])
{
   double max_y = DBL_MIN;

   for (int i = 0; i < 4; i++) {
      double max = hist[i]->GetBinContent(hist[i]->GetMaximumBin());
      if (max > max_y)
         max_y = max;
   }

   return max_y;
}

void analyze()
{
   std::ifstream ifs("data.dat");
   std::string line;

   TH1D* combined[4];
   TH1D* valid[4];
   TH1D* invalid[4];

   ULong64_t integral_valid[4];
   ULong64_t integral_invalid[4];
   ULong64_t integral_combined[4];

   combined[0] = new TH1D("combined[0]", "Softsusy"    , 20, 0., 2.);
   combined[1] = new TH1D("combined[1]", "FlexibleSUSY", 20, 0., 2.);
   combined[2] = new TH1D("combined[2]", "SPheno"      , 20, 0., 2.);
   combined[3] = new TH1D("combined[3]", "SPhenoMSSM"  , 20, 0., 2.);

   valid[0] = new TH1D("valid[0]", "Softsusy"    , 20, 0., 2.);
   valid[1] = new TH1D("valid[1]", "FlexibleSUSY", 20, 0., 2.);
   valid[2] = new TH1D("valid[2]", "SPheno"      , 20, 0., 2.);
   valid[3] = new TH1D("valid[3]", "SPhenoMSSM"  , 20, 0., 2.);

   invalid[0] = new TH1D("invalid[0]", "Softsusy"    , 20, 0., 2.);
   invalid[1] = new TH1D("invalid[1]", "FlexibleSUSY", 20, 0., 2.);
   invalid[2] = new TH1D("invalid[2]", "SPheno"      , 20, 0., 2.);
   invalid[3] = new TH1D("invalid[3]", "SPhenoMSSM"  , 20, 0., 2.);

   for (int i = 0; i < 4; i++) {
      combined[i]->SetStats(0);
      valid[i]->SetStats(0);
      invalid[i]->SetStats(0);
   }

   // initialize integrals
   for (int i = 0; i < 4; i++) {
      integral_valid[i] = 0;
      integral_invalid[i] = 0;
      integral_combined[i] = 0;
   }

   while (getline(ifs,line)) {
      std::istringstream input(line);
      std::string word;
      input >> word;
      if (word.find("#") == std::string::npos) {
         std::istringstream kk(line);
         double m0, m12, tanb, sgnmu, a0;
         double time[4];
         int error[4];

         kk >> m0 >> m12 >> tanb >> sgnmu >> a0;
         for (int i = 0; i < 4; i++) {
            try {
               kk >> time[i] >> error[i];
            } catch (...) {
               cout << "Error: time and error are not convertible for"
                  " spectrum generator " << i << endl;
            }
         }

         for (int i = 0; i < 4; i++) {
            combined[i]->Fill(time[i]);
            integral_combined[i]++;
            if (error[i] == 0) {
               valid[i]->Fill(time[i]);
               integral_valid[i]++;
            } else {
               invalid[i]->Fill(time[i]);
               integral_invalid[i]++;
            }
         }
      }
   }

   // rescale
   for (int i = 0; i < 4; i++) {
      valid[i]->Scale(1./valid[i]->Integral());
      invalid[i]->Scale(1./invalid[i]->Integral());
      combined[i]->Scale(1./combined[i]->Integral());
   }

   combined[0]->SetTitle("CMSSM execution time/s (valid + invalid spectrum)");
   combined[0]->GetXaxis()->SetTitle("execution time / s");

   valid[0]->SetTitle("CMSSM execution time/s (valid spectrum)");
   valid[0]->GetXaxis()->SetTitle("execution time / s");

   invalid[0]->SetTitle("CMSSM execution time/s (invalid spectrum)");
   invalid[0]->GetXaxis()->SetTitle("execution time / s");

   TCanvas* canvas = new TCanvas("canvas", "CMSSM benchmark", 800, 600);
   canvas->Divide(2,2);

   canvas->cd(1);
   valid[0]->GetYaxis()->SetRangeUser(0., 1.);
   for (int i = 0; i < 4; i++) {
      valid[i]->SetLineColor(i + 1);
      valid[i]->SetLineWidth(3);
      valid[i]->Draw(i == 0 ? "" : "same");
   }

   TLegend *valid_legend = new TLegend(0.6,0.7,0.9,0.9);
   valid_legend->SetFillColor(0);
   valid_legend->AddEntry(valid[0], TString("Softsusy")
                          + " (" + TString::ULLtoa(integral_valid[0],10) + ")", "f");
   valid_legend->AddEntry(valid[1], valid[1]->GetTitle()
                          + TString(" (") + TString::ULLtoa(integral_valid[1],10) + ")", "f");
   valid_legend->AddEntry(valid[2], valid[2]->GetTitle()
                          + TString(" (") + TString::ULLtoa(integral_valid[2],10) + ")", "f");
   valid_legend->AddEntry(valid[3], valid[3]->GetTitle()
                          + TString(" (") + TString::ULLtoa(integral_valid[3],10) + ")", "f");
   valid_legend->Draw();

   canvas->cd(2);
   invalid[0]->GetYaxis()->SetRangeUser(0., 1.);
   for (int i = 0; i < 4; i++) {
      invalid[i]->SetLineColor(i + 1);
      invalid[i]->SetLineWidth(3);
      invalid[i]->Draw(i == 0 ? "" : "same");
   }

   TLegend *invalid_legend = new TLegend(0.6,0.7,0.9,0.9);
   invalid_legend->SetFillColor(0);
   invalid_legend->AddEntry(invalid[0], TString("Softsusy")
                          + " (" + TString::ULLtoa(integral_invalid[0],10) + ")", "f");
   invalid_legend->AddEntry(invalid[1], invalid[1]->GetTitle()
                          + TString(" (") + TString::ULLtoa(integral_invalid[1],10) + ")", "f");
   invalid_legend->AddEntry(invalid[2], invalid[2]->GetTitle()
                          + TString(" (") + TString::ULLtoa(integral_invalid[2],10) + ")", "f");
   invalid_legend->AddEntry(invalid[3], invalid[3]->GetTitle()
                          + TString(" (") + TString::ULLtoa(integral_invalid[3],10) + ")", "f");
   invalid_legend->Draw();

   canvas->cd(3);
   combined[0]->GetYaxis()->SetRangeUser(0., 1.);
   for (int i = 0; i < 4; i++) {
      combined[i]->SetLineColor(i + 1);
      combined[i]->SetLineWidth(3);
      combined[i]->Draw(i == 0 ? "" : "same");
   }

   TLegend *combined_legend = new TLegend(0.6,0.7,0.9,0.9);
   combined_legend->SetFillColor(0);
   combined_legend->AddEntry(combined[0], TString("Softsusy")
                          + " (" + TString::ULLtoa(integral_combined[0],10) + ")", "f");
   combined_legend->AddEntry(combined[1], combined[1]->GetTitle()
                          + TString(" (") + TString::ULLtoa(integral_combined[1],10) + ")", "f");
   combined_legend->AddEntry(combined[2], combined[2]->GetTitle()
                          + TString(" (") + TString::ULLtoa(integral_combined[2],10) + ")", "f");
   combined_legend->AddEntry(combined[3], combined[3]->GetTitle()
                          + TString(" (") + TString::ULLtoa(integral_combined[3],10) + ")", "f");
   combined_legend->Draw();

   canvas->Draw();
}
