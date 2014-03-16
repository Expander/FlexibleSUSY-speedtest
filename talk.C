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

void talk(const std::string& filename = "data.dat",
          const std::string& title = "CMSSM spectrum generator run-time")
{
   std::ifstream ifs(filename.c_str());
   std::string line;

   if (!ifs.good()) {
      cout << "Error: could not open file: " << filename << endl;
      return;
   }

   const int NSG = 4;
   const double range_start = 0., range_stop = 1.;
   TH1D* combined[NSG];
   TH1D* valid[NSG];
   TH1D* invalid[NSG];

   ULong64_t integral_valid[NSG];
   ULong64_t integral_invalid[NSG];
   ULong64_t integral_combined[NSG];

   combined[0] = new TH1D("combined[0]", "Softsusy"    , 20, range_start, range_stop);
   combined[1] = new TH1D("combined[1]", "FlexibleSUSY", 20, range_start, range_stop);
   combined[2] = new TH1D("combined[2]", "SPheno"      , 20, range_start, range_stop);
   combined[3] = new TH1D("combined[3]", "SPhenoMSSM"  , 20, range_start, range_stop);

   valid[0] = new TH1D("valid[0]", "Softsusy 3.4.0"    , 20, range_start, range_stop);
   valid[1] = new TH1D("valid[1]", "FlexibleSUSY 0.5.3", 20, range_start, range_stop);
   valid[2] = new TH1D("valid[2]", "SPheno 3.2.4"      , 20, range_start, range_stop);
   valid[3] = new TH1D("valid[3]", "SPhenoMSSM 4.1.0"  , 20, range_start, range_stop);

   invalid[0] = new TH1D("invalid[0]", "Softsusy"    , 20, range_start, range_stop);
   invalid[1] = new TH1D("invalid[1]", "FlexibleSUSY", 20, range_start, range_stop);
   invalid[2] = new TH1D("invalid[2]", "SPheno"      , 20, range_start, range_stop);
   invalid[3] = new TH1D("invalid[3]", "SPhenoMSSM"  , 20, range_start, range_stop);

   for (int i = 0; i < NSG; i++) {
      combined[i]->SetStats(0);
      valid[i]->SetStats(0);
      invalid[i]->SetStats(0);
   }

   // initialize integrals
   for (int i = 0; i < NSG; i++) {
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
         double time[NSG];
         int error[NSG];

         kk >> m0 >> m12 >> tanb >> sgnmu >> a0;
         for (int i = 0; i < NSG; i++) {
            try {
               kk >> time[i] >> error[i];
            } catch (...) {
               cout << "Error: time and error are not convertible for"
                  " spectrum generator " << i << endl;
            }
         }

         for (int i = 0; i < NSG; i++) {
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
   for (int i = 0; i < NSG; i++) {
      valid[i]->Scale(1./valid[i]->Integral());
      invalid[i]->Scale(1./invalid[i]->Integral());
      combined[i]->Scale(1./combined[i]->Integral());
   }

   combined[0]->SetTitle("CMSSM run-time/s (valid + invalid spectrum)");
   combined[0]->GetXaxis()->SetTitle("run-time / s");

   valid[0]->SetTitle(title.c_str());
   valid[0]->GetXaxis()->SetTitle("run-time / s");

   invalid[0]->SetTitle("CMSSM run-time/s (invalid spectrum)");
   invalid[0]->GetXaxis()->SetTitle("run-time / s");

   TCanvas* canvas = new TCanvas("canvas", "CMSSM benchmark", 800, 600);
   // canvas->Divide(2,2);

   canvas->cd(1);
   valid[0]->GetYaxis()->SetRangeUser(range_start, range_stop);
   for (int i = 0; i < NSG; i++) {
      valid[i]->SetLineWidth(3);
      valid[i]->Draw(i == 0 ? "" : "same");
   }

   valid[0]->SetLineColor(1); valid[0]->SetFillColor(1); valid[0]->SetFillStyle(3004);
   valid[1]->SetLineColor(3); valid[1]->SetFillColor(3); valid[1]->SetFillStyle(3001);
   valid[2]->SetLineColor(4); valid[2]->SetFillColor(4); valid[2]->SetFillStyle(3005);
   valid[3]->SetLineColor(2); valid[3]->SetFillColor(2); valid[3]->SetFillStyle(3006);

   TLegend *valid_legend = new TLegend(0.5,0.7,0.9,0.9);
   valid_legend->SetFillColor(0);
   valid_legend->AddEntry(valid[1], TString::Format("%s (%3.3fs)", valid[1]->GetTitle(), valid[1]->GetMean()), "f");
   valid_legend->AddEntry(valid[0], TString::Format("%s (%3.3fs)", "Softsusy 3.4.0"    , valid[0]->GetMean()), "f");
   valid_legend->AddEntry(valid[2], TString::Format("%s (%3.3fs)", valid[2]->GetTitle(), valid[2]->GetMean()), "f");
   valid_legend->AddEntry(valid[3], TString::Format("%s (%3.3fs)", valid[3]->GetTitle(), valid[3]->GetMean()), "f");
   valid_legend->Draw();

   canvas->Draw();
   canvas->SaveAs("benchmark.pdf");
}
