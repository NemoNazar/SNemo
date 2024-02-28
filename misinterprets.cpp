// Mi headers
#include "/sps/nemo/scratch/nnemoshc/MiModule/include/MiEvent.h"
#include "/sps/nemo/scratch/nnemoshc/MiModule/include/MiSDVisuHit.h"
#include "/sps/nemo/scratch/nnemoshc/MiModule/include/MiFilters.h"

#include <string>
#include <iostream>
#include <sstream>
#include "TCanvas.h"
#include "TStyle.h"
#include "TH1.h"
#include "TGaxis.h"
#include "TRandom.h"
R__LOAD_LIBRARY(/sps/nemo/scratch/nnemoshc/MiModule/lib/libMiModule.so);
int step_in_gas3(MiEvent*  _eve) // returns the step position of the hit_step when it first enters tracking gas
{
	for (int step = 0; step < _eve->getSD()->getvisuhitv()->size() ; step++) // iterating over the steps of the simulation
	{

		if( // check whether the particle belongs to 1st track, is in gas, has left volume (source) and entered new vol (gas)
			_eve->getSD()->getvisuhitv()->at(step).getMaterial()      == "tracking_gas" 		 
		  )
		{
			return step;
		}
	}
	return -1; //return -1 if the particle never left source foil (happens when we get "fakeItTillYouMakeIt events")
}
int name_charge(string partname){
    if (partname=="e-"){
        return -1;
    }
    else if(partname=="gamma"){
        return 0;
    }
    else if(partname=="e+"){
        return 1;
    }
    else if(partname=="alpha"){
        return 2;
    }
        cout<<partname;
        return 3;

}

void misinterprets()
{

    TCanvas *c1 = new TCanvas("c1", "H1s", 1950, 540);
    auto h1 = new TH2D("misinterprets particles", "1 track per event", 5, -1.5, 3.5, 5, -1.5, 3.5);
    auto h2 = new TH2D("misinterprets particles", "2 track per event", 5, -1.5, 3.5, 5, -1.5, 3.5);
    auto h3 = new TH2D("misinterprets particles", "3 track per event", 5, -1.5, 3.5, 5, -1.5, 3.5);

    MiEvent *eve = new MiEvent();
    MiFilters *miFilter;

    const char *inFile1 = ".././data/";
    const char *inFile2 = "AllD.root";
    for (int n = 1; n < 2; n++)
    { // loop over all files

        stringstream ssInPath1;
        ssInPath1 << inFile1;
        //ssInPath1 << n;
        ssInPath1 << inFile2;

        cout << ssInPath1.str().c_str() << endl;

        TFile *f = new TFile(ssInPath1.str().c_str());

        TTree *s = (TTree *)f->Get("Event");

        s->SetBranchAddress("Eventdata", &eve);
        for(UInt_t i=0; i < s->GetEntries(); i++)	// Loop over all events
        //for (UInt_t i = 0; i < 1000; i++) // Loop over few events for test code
        {
            s->GetEntry(i);
            // eve->print();
            int Ntracks =0;
            int trackID;

            for (UInt_t step = step_in_gas3(eve); step < eve->getSD()->getvisuhitv()->size(); step++)
            {
                if (eve->getPTD()->getpartv()->size() == 1)
                {
                    if(eve->getSD()->getvisuhitv()->at(step).getTrackID()==trackID){continue;}
                    if( Ntracks>=eve->getPTD()->getpartv()->size())break;
                    //if(eve->getPTD()->getpart(Ntracks)->getvertex(0)->getr()==eve->getSD()->getvisuhitv()->at(step).getStart()){
                        h1->Fill(name_charge(eve->getSD()->getvisuhitv()->at(step).getParticleName()), eve->getPTD()->getpart(Ntracks)->getcharge());
                        Ntracks ++;
                    //}
                    trackID = eve->getSD()->getvisuhitv()->at(step).getTrackID();
                    
                }
                else if (eve->getPTD()->getpartv()->size() == 2)
                {
                    if(eve->getSD()->getvisuhitv()->at(step).getTrackID()==trackID){continue;}
                    if( Ntracks>=eve->getPTD()->getpartv()->size())break;
                    //if(eve->getPTD()->getpart(Ntracks)->getvertex(0)->getr()==eve->getSD()->getvisuhitv()->at(step).getStart()){
                        h2->Fill(name_charge(eve->getSD()->getvisuhitv()->at(step).getParticleName()), eve->getPTD()->getpart(Ntracks)->getcharge());
                        Ntracks ++;
                    //}
                    trackID = eve->getSD()->getvisuhitv()->at(step).getTrackID();
                }
                else if (eve->getPTD()->getpartv()->size() == 3)
                {
                   if(eve->getSD()->getvisuhitv()->at(step).getTrackID()==trackID){continue;}
                    if( Ntracks>=eve->getPTD()->getpartv()->size())break;
                    //if(eve->getPTD()->getpart(Ntracks)->getvertexv().getr()==eve->getSD()->getvisuhitv()->at(step).getStart()){
                        h3->Fill(name_charge(eve->getSD()->getvisuhitv()->at(step).getParticleName()), eve->getPTD()->getpart(Ntracks)->getcharge());
                        Ntracks ++;
                    //}
                    trackID = eve->getSD()->getvisuhitv()->at(step).getTrackID();
                }
            }
        }
    }
    c1->Divide(3);
    
   h1->GetXaxis()->SetTitle("Simulated particle");
   h1->GetYaxis()->SetTitle("Calibrated particle");

    c1->cd(1);
    //auto palette1 = new TPaletteAxis(2.55,-1.5,2.7,1.25,h1);
    //h1->GetListOfFunctions()->Add(palette1);
    h1->Scale(1. / h1->Integral()*100, "width");
    h1->Draw("col");
    h1->GetXaxis()->SetBinLabel(1, "e-");
    h1->GetXaxis()->SetBinLabel(2, "#gamma"); // Unicode representation of gamma symbol
    h1->GetXaxis()->SetBinLabel(3, "e+");
    h1->GetXaxis()->SetBinLabel(4, "#alpha"); // Unicode representation of alpha symbol
    h1->GetXaxis()->SetBinLabel(5, "Uk");

    // Set custom labels for the Y axis
    h1->GetYaxis()->SetBinLabel(1, "e-");
    h1->GetYaxis()->SetBinLabel(2, "#gamma"); // Unicode representation of gamma symbol
    h1->GetYaxis()->SetBinLabel(3, "e+");
    h1->GetYaxis()->SetBinLabel(4, "#alpha"); // Unicode representation of alpha symbol
    h1->GetYaxis()->SetBinLabel(5, "Uk");

    // Set text size for axis labels
    h1->GetXaxis()->SetLabelSize(0.06); // X axis label size
    h1->GetYaxis()->SetLabelSize(0.06); // Y axis label size
    for (int xbin = 1; xbin <= h1->GetNbinsX(); ++xbin) {
        for (int ybin = 1; ybin <= h1->GetNbinsY(); ++ybin) {
            double binContent = h1->GetBinContent(xbin, ybin); // Get the content of the bin
            TString textContent = (binContent == 0) ? "0%" : Form("%.2f%%", binContent); // Format the text content with "%" symbol
            TText *text = new TText(h1->GetXaxis()->GetBinCenter(xbin), h1->GetYaxis()->GetBinCenter(ybin), textContent.Data()); // Create text with percentage value
            text->SetTextSize(0.05); // Set text size
            text->SetTextAlign(22); // Center-align text in cell
            text->Draw(); // Draw text in bin
        }
    }
    
    c1->Update();


   h2->GetXaxis()->SetTitle("Simulated particle");
   h2->GetYaxis()->SetTitle("Calibrated particle");
    h2->GetXaxis()->SetBinLabel(1, "e-");
    h2->GetXaxis()->SetBinLabel(2, "#gamma"); // Unicode representation of gamma symbol
    h2->GetXaxis()->SetBinLabel(3, "e+");
    h2->GetXaxis()->SetBinLabel(4, "#alpha"); // Unicode representation of alpha symbol
    h2->GetXaxis()->SetBinLabel(5, "Uk");
    
    // Set custom labels for the Y axis
    h2->GetYaxis()->SetBinLabel(1, "e-");
    h2->GetYaxis()->SetBinLabel(2, "#gamma"); // Unicode representation of gamma symbol
    h2->GetYaxis()->SetBinLabel(3, "e+");
    h2->GetYaxis()->SetBinLabel(4, "#alpha"); // Unicode representation of alpha symbol
    h2->GetYaxis()->SetBinLabel(5, "Uk");

    // Set text size for axis labels
    h2->GetXaxis()->SetLabelSize(0.06); // X axis label size
    h2->GetYaxis()->SetLabelSize(0.06); // Y axis label size
    c1->cd(2);
    //auto palette2 = new TPaletteAxis(2.55,-1.5,2.7,1.25,h2);
    //h2->GetListOfFunctions()->Add(palette2);
    h2->Scale(1. / h2->Integral()*100, "width");
    h2->Draw("col");
    for (int xbin = 1; xbin <= h2->GetNbinsX(); ++xbin) {
        for (int ybin = 1; ybin <= h2->GetNbinsY(); ++ybin) {
            double binContent = h2->GetBinContent(xbin, ybin); // Get the content of the bin
            TString textContent = (binContent == 0) ? "0%" : Form("%.2f%%", binContent); // Format the text content with "%" symbol
            TText *text = new TText(h2->GetXaxis()->GetBinCenter(xbin), h2->GetYaxis()->GetBinCenter(ybin), textContent.Data()); // Create text with percentage value
            text->SetTextSize(0.05); // Set text size
            text->SetTextAlign(22); // Center-align text in cell
            text->Draw(); // Draw text in bin
        }
    }
    c1->Update();

    // Update the canvas

   h3->GetXaxis()->SetTitle("Simulated particle");
   h3->GetYaxis()->SetTitle("Calibrated particle");
        h3->GetXaxis()->SetBinLabel(1, "e-");
    h3->GetXaxis()->SetBinLabel(2, "#gamma"); // Unicode representation of gamma symbol
    h3->GetXaxis()->SetBinLabel(3, "e+");
    h3->GetXaxis()->SetBinLabel(4, "#alpha"); // Unicode representation of alpha symbol
    h3->GetXaxis()->SetBinLabel(5, "Uk");
    // Set custom labels for the Y axis
    h3->GetYaxis()->SetBinLabel(1, "e-");
    h3->GetYaxis()->SetBinLabel(2, "#gamma"); // Unicode representation of gamma symbol
    h3->GetYaxis()->SetBinLabel(3, "e+");
    h3->GetYaxis()->SetBinLabel(4, "#alpha"); // Unicode representation of alpha symbol
    h3->GetYaxis()->SetBinLabel(5, "Uk");
    // Set text size for axis labels
    h3->GetXaxis()->SetLabelSize(0.06); // X axis label size
    h3->GetYaxis()->SetLabelSize(0.06); // Y axis label size
    c1->cd(3);
    h3->Scale(1. / h3->Integral()*100, "width");
    //auto palette3 = new TPaletteAxis(2.55,-1.5,2.7,1.25,h3);
    //h3->GetListOfFunctions()->Add(palette3);
    h3->Draw("col");
    for (int xbin = 1; xbin <= h3->GetNbinsX(); ++xbin) {
        for (int ybin = 1; ybin <= h3->GetNbinsY(); ++ybin) {
            double binContent = h3->GetBinContent(xbin, ybin); // Get the content of the bin
            TString textContent = (binContent == 0) ? "0%" : Form("%.2f%%", binContent); // Format the text content with "%" symbol
            TText *text = new TText(h3->GetXaxis()->GetBinCenter(xbin), h3->GetYaxis()->GetBinCenter(ybin), textContent.Data()); // Create text with percentage value
            text->SetTextSize(0.05); // Set text size
            text->SetTextAlign(22); // Center-align text in cell
            text->Draw(); // Draw text in bin
        }
    }

    // Update the canvas
    c1->Update();
    
    
}