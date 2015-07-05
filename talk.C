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

struct HistStyle {
   int linecolor, fillcolor, fillstyle;
} hist_style[] = {
   {3, 3, 3004},
   {4, 4, 3005},
   {2, 2, 3007},
   {8, 8, 3001}
};

void draw(const std::string& title, const std::vector<TH1D*>& histograms,
          const std::string& filename)
{
   if (histograms.empty() ||
       histograms.size() > (sizeof(hist_style)/sizeof(hist_style[0])))
      return;

   TCanvas* canvas = new TCanvas("canvas", title.c_str(), 800, 600);
   canvas->cd(1)->SetTitle(title.c_str());

   histograms[0]->GetYaxis()->SetRangeUser(0., 1.);
   for (int i = 0; i < histograms.size(); i++) {
      histograms[i]->SetLineWidth(3);
      histograms[i]->SetLineColor(hist_style[i].linecolor);
      histograms[i]->SetFillColor(hist_style[i].fillcolor);
      histograms[i]->SetFillStyle(hist_style[i].fillstyle);
      histograms[i]->Draw(i == 0 ? "" : "same");
   }

   TLegend *legend = new TLegend(0.4,0.7,0.9,0.9);
   legend->SetFillColor(0);
   for (int i = 0; i < histograms.size(); i++) {
      legend->AddEntry(histograms[i],
                       TString::Format("%s (%3.3fs)", histograms[i]->GetTitle(), histograms[i]->GetMean()),
                       "f");
   }
   legend->Draw();

   histograms[0]->SetTitle(title.c_str());
   histograms[0]->GetXaxis()->SetTitle("run-time / s");

   canvas->Draw();
   canvas->SaveAs(TString(filename) + ".pdf");
   canvas->SaveAs(TString(filename) + ".png");

   delete legend;
   delete canvas;
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

   const int NSG = 5;
   const double range_start = 0., range_stop = 1.;
   TH1D* combined[NSG];
   TH1D* valid[NSG];
   TH1D* invalid[NSG];

   ULong64_t integral_valid[NSG];
   ULong64_t integral_invalid[NSG];
   ULong64_t integral_combined[NSG];

   combined[0] = new TH1D("combined[0]", "Softsusy 3.6.2"        , 20, range_start, range_stop);
   combined[1] = new TH1D("combined[1]", "FlexibleSUSY-CMSSM"    , 20, range_start, range_stop);
   combined[2] = new TH1D("combined[2]", "SPheno 3.3.7"          , 20, range_start, range_stop);
   combined[3] = new TH1D("combined[3]", "SPhenoMSSM"            , 20, range_start, range_stop);
   combined[4] = new TH1D("combined[4]", "FlexibleSUSY-CMSSMNoFV", 20, range_start, range_stop);

   valid[0] = new TH1D("valid[0]", "Softsusy 3.6.2"              , 20, range_start, range_stop);
   valid[1] = new TH1D("valid[1]", "FlexibleSUSY-CMSSM 1.2.0"    , 20, range_start, range_stop);
   valid[2] = new TH1D("valid[2]", "SPheno 3.3.7"                , 20, range_start, range_stop);
   valid[3] = new TH1D("valid[3]", "SPhenoMSSM 3.3.7/4.5.8"      , 20, range_start, range_stop);
   valid[4] = new TH1D("valid[4]", "FlexibleSUSY-CMSSMNoFV 1.2.0", 20, range_start, range_stop);

   invalid[0] = new TH1D("invalid[0]", "Softsusy"              , 20, range_start, range_stop);
   invalid[1] = new TH1D("invalid[1]", "FlexibleSUSY-CMSSM"    , 20, range_start, range_stop);
   invalid[2] = new TH1D("invalid[2]", "SPheno"                , 20, range_start, range_stop);
   invalid[3] = new TH1D("invalid[3]", "SPhenoMSSM"            , 20, range_start, range_stop);
   invalid[4] = new TH1D("invalid[4]", "FlexibleSUSY-CMSSMNoFV", 20, range_start, range_stop);

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
      if (word == "" || word.find("#") == std::string::npos) {
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

   std::vector<TH1D*> nofv_hists;
   nofv_hists.push_back(valid[4]); // FS
   nofv_hists.push_back(valid[2]); // SP
   nofv_hists.push_back(valid[0]); // SS

   draw("CMSSM w/o sfermion flavour violation", nofv_hists, "benchmark_nofv");

   std::vector<TH1D*> fv_hists;
   fv_hists.push_back(valid[1]); // FS
   fv_hists.push_back(valid[3]); // S/SP

   draw("CMSSM w/ sfermion flavour violation", fv_hists, "benchmark_fv");
}
