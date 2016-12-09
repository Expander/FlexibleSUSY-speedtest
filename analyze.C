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

void analyze(const std::string& filename = "data.dat", double xmin = 0, double xmax = 1.)
{
   std::ifstream ifs(filename.c_str());
   std::string line;

   if (!ifs.good()) {
      cout << "Error: could not open file: " << filename << endl;
      return;
   }

   const int NSG = 4;
   TH1D* combined[NSG];
   TH1D* valid[NSG];
   TH1D* invalid[NSG];

   ULong64_t integral_valid[NSG];
   ULong64_t integral_invalid[NSG];
   ULong64_t integral_combined[NSG];

   combined[0] = new TH1D("combined[0]", "Softsusy"    , 20, xmin, xmax);
   combined[1] = new TH1D("combined[1]", "FlexibleSUSY", 20, xmin, xmax);
   combined[2] = new TH1D("combined[2]", "SPheno"      , 20, xmin, xmax);
   combined[3] = new TH1D("combined[3]", "SPhenoMSSM"  , 20, xmin, xmax);

   valid[0] = new TH1D("valid[0]", "Softsusy"    , 20, xmin, xmax);
   valid[1] = new TH1D("valid[1]", "FlexibleSUSY", 20, xmin, xmax);
   valid[2] = new TH1D("valid[2]", "SPheno"      , 20, xmin, xmax);
   valid[3] = new TH1D("valid[3]", "SPhenoMSSM"  , 20, xmin, xmax);

   invalid[0] = new TH1D("invalid[0]", "Softsusy"    , 20, xmin, xmax);
   invalid[1] = new TH1D("invalid[1]", "FlexibleSUSY", 20, xmin, xmax);
   invalid[2] = new TH1D("invalid[2]", "SPheno"      , 20, xmin, xmax);
   invalid[3] = new TH1D("invalid[3]", "SPhenoMSSM"  , 20, xmin, xmax);

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
         bool stream_ok = true;

         kk >> m0 >> m12 >> tanb >> sgnmu >> a0;
         for (int i = 0; i < NSG; i++) {
            try {
               stream_ok = kk.good();
               if (stream_ok)
                  kk >> time[i];
               stream_ok = kk.good();
               if (stream_ok)
                  kk >> error[i];
            } catch (...) {
               cout << "Error: time and error are not convertible for"
                  " spectrum generator " << i << endl;
               stream_ok = false;
            }
         }

         if (!stream_ok) {
            cout << "Error: invalid line: " << line << '\n';
            continue;
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
   for (int i = 0; i < NSG; i++) {
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
   for (int i = 0; i < NSG; i++) {
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
   for (int i = 0; i < NSG; i++) {
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
   canvas->SaveAs("benchmark.pdf");
}
