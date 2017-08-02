#ifndef _READXML_H_
#define _READXML_H_

#include "../includes/uti.h"
#include "../includes/TMVAClassification.h"
#include "../includes/cfout.h"

const int NmaxFonll = 11; //fonll data points number
float fcentral[NmaxFonll],fpt[NmaxFonll];
const int NEff = 100;
const int nMVA = 50;
Float_t minMVA = -1;
Float_t maxMVA = 1;
Float_t widMVA = (maxMVA-minMVA)/nMVA;
Float_t gmvaBins[nMVA+1];

Double_t norm = 0.557; // FF of c->D0, i.e., D fraction
Double_t BR =0.0387;
Double_t deltapt = 0.10;

Float_t massD = 1.865;
Float_t dmassDsignal = 0.05;
Float_t dmassDsidbandL = 0.1;
Float_t dmassDsidbandH = 0.13;
Float_t massDsignalL = massD - dmassDsignal;
Float_t massDsignalH = massD + dmassDsignal;

TString outputresult = "results/fresult";
TString outputfonll = "plots/cfonll";
TString outputmvadis = "plots/cmvadistribution";
TString outputefficiency = "plots/cefficiency";
TString outputsignificance = "plots/csignificance";

int calRatio(TTree* signal, TTree* background, TTree* generated, 
             TString cuts, TString cutb, TString cutg, TString weight,
             Float_t ptmin, Float_t ptmax,
             TString fonllfile, Float_t lumi, Float_t raa, TString outputname,
             Float_t* results)
{
  // background sideband -> signal region
  TH1D* hMassD = new TH1D("hMassD",";D mass (GeV/c^{2});Background Entries",50,0,10);
  background->Project("hMassD","Dmass",cutb);
  Float_t nB = hMassD->Integral()*dmassDsignal/(dmassDsidbandH-dmassDsidbandL);

  // fonll predictions
  std::ifstream getdata(fonllfile);
  if(!getdata.is_open()) 
    {
      std::cout<<"Opening fonll file fails"<<std::endl;
      return 1;
    }
  Float_t tem;
  Int_t nbin=0;
  while (!getdata.eof())
    {
      getdata>>fpt[nbin]>>fcentral[nbin]>>tem>>tem>>tem>>tem>>tem>>tem>>tem>>tem;
      if(fpt[nbin]>=ptmin&&fpt[nbin]<=ptmax) nbin++;
    }
  TH1D* hfonll = new TH1D("hfonll",";D p_{T} (GeV/c);FONLL differential xsection",nbin-1,fpt);
  for(int i=0;i<nbin;i++)
    {
      hfonll->SetBinContent(i,fcentral[i]);
    }
  TCanvas* cfonll = new TCanvas("cfonll","",600,600);
  hfonll->Draw();
  cfonll->SaveAs(Form("%s_%s.pdf",outputfonll.Data(),outputname.Data()));

  TH1D* hrec = new TH1D("hrec",";D p_{T} (GeV/c);Signal reco entries",nbin-1,fpt);
  TH1D* hgen = new TH1D("hgen",";D p_{T} (GeV/c);Generated entries",nbin-1,fpt);
  TH1D* heff = new TH1D("heff",";D p_{T} (GeV/c);Prefilter efficiency",nbin-1,fpt);
  signal->Project("hrec","Dpt",Form("%s*(%s)",weight.Data(),cuts.Data()));
  generated->Project("hgen","Gpt",Form("%s*(%s)",weight.Data(),cutg.Data()));
  cout << "Total Signal = " << hrec->Integral() << endl;
  cout << "Total Generated = " << hgen->Integral() << endl;

  heff->Divide(hrec,hgen,1.,1.,"B");
  TH1D* htheoryreco = new TH1D("htheoryreco","",nbin-1,fpt);
  htheoryreco->Multiply(heff,hfonll,1,1,"B");
  Float_t nS = htheoryreco->Integral()*norm*BR*deltapt*lumi*raa*2; // x2

  results[0] = nB;
  results[1] = nS;

  return 0;
}

#endif
