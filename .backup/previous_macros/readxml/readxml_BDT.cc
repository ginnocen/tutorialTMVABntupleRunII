using namespace std;
#include "uti.h"
#include "readxml_BDT.h"
#include "Tools.h"
#include "../prefilter.h"

TString cuts;
TString cutb;
TString cutg;
TString weight="1";

void readxml_BDT(TString inputSname, TString inputBname,
                 Int_t pbpb=1, TString mva="BDT", Float_t ptMin=7., Float_t ptMax=10., Float_t RAA=1.)
{
  gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0);
  gStyle->SetEndErrorSize(0);
  gStyle->SetMarkerStyle(20);
  gStyle->SetTextSize(0.05);
  gStyle->SetTextFont(42);
  gStyle->SetPadRightMargin(0.043);
  gStyle->SetPadLeftMargin(0.18);
  gStyle->SetPadTopMargin(0.1);
  gStyle->SetPadBottomMargin(0.145);
  gStyle->SetTitleX(.0f);

  void calRatio(TTree* signal, TTree* background, TTree* generated, Float_t* results, Bool_t verbose=false);
  ptmin = ptMin;
  ptmax = ptMax;
  isPbPb = (bool)pbpb;
  MVAtype = (TString)mva;
  raa = isPbPb?RAA:1.;
  mycuts = isPbPb?mycuts_PP:mycuts_pp;
  mycutb = isPbPb?mycutb_PP:mycutb_pp;
  mycutg = isPbPb?mycutg_PP:mycutg_pp;
  colsyst = isPbPb?"PbPb":"pp";

  cuts = isPbPb?Form("(%s)&&Bpt>%f&&Bpt<%f&&hiBin>=0&&hiBin<=200",mycuts.Data(),ptmin,ptmax):Form("(%s)&&Bpt>%f&&Bpt<%f",mycuts.Data(),ptmin,ptmax);
  cutb = isPbPb?Form("(%s)&&Bpt>%f&&Bpt<%f&&hiBin>=0&&hiBin<=200",mycutb.Data(),ptmin,ptmax):Form("(%s)&&Bpt>%f&&Bpt<%f",mycutb.Data(),ptmin,ptmax);
  cutg = isPbPb?Form("(%s)&&Gpt>%f&&Gpt<%f&&hiBin>=0&&hiBin<=200",mycutg.Data(),ptmin,ptmax):Form("(%s)&&Gpt>%f&&Gpt<%f",mycutg.Data(),ptmin,ptmax);

  TLatex* texPar = new TLatex(0.18,0.93, "PbPb 5.02 TeV B^{+}");
  texPar->SetNDC();
  texPar->SetTextAlign(12);
  texPar->SetTextSize(0.04);
  texPar->SetTextFont(42);
  TLatex* texPtY = new TLatex(0.96,0.93, Form("|y|<2.4, %.1f<p_{T}<%.1f GeV/c",ptmin,ptmax));
  texPtY->SetNDC();
  texPtY->SetTextAlign(32);
  texPtY->SetTextSize(0.04);
  texPtY->SetTextFont(42);

  TString ptstring = Form("(%.1f,%.1f)",ptmin,ptmax);
  cout<<endl;
  cout<<" ╒══════════════════════════════════════════════════╕"<<endl;
  cout<<" |               Cut Opt Configuration              |"<<endl;
  cout<<" ├────────────┬────────────────────────────┬────────┤"<<endl;
  cout<<" | "<<setiosflags(ios::left)<<setw(10)<<"Pt"<<" | "<<setiosflags(ios::left)<<setw(26)<<ptstring<<" | "<<setiosflags(ios::left)<<setw(6)<<" "<<" |"<<endl;
  cout<<" ├────────────┼────────────────────────────┼────────┤"<<endl;
  cout<<" | "<<setiosflags(ios::left)<<setw(10)<<"Raa"<<" | "<<setiosflags(ios::left)<<setw(26)<<raa<<" | "<<setiosflags(ios::left)<<setw(6)<<" "<<" |"<<endl;
  cout<<" ╘════════════╧════════════════════════════╧════════╛"<<endl;
  cout<<endl;

  TFile *inputS = new TFile(inputSname.Data());
  TFile *inputB = new TFile(inputBname.Data());

  TTree* background = (TTree*)inputB->Get("ntKp");
  background->AddFriend("ntHlt");
  background->AddFriend("ntHi");
  background->AddFriend("ntSkim");
  background->AddFriend("mvaTree");

  TTree* signal = (TTree*)inputS->Get("ntKp");
  signal->AddFriend("ntHlt");
  signal->AddFriend("ntHi");
  signal->AddFriend("ntSkim");
  signal->AddFriend("mvaTree");

  TTree* generated = (TTree*)inputS->Get("ntGen");
  generated->AddFriend("ntHlt");
  generated->AddFriend("ntHi");

  const Int_t nBDT = 50;//
  Float_t minBDT = -1;//
  Float_t maxBDT = 1;//
  Float_t widBDT = (maxBDT-minBDT)/nBDT;
  Float_t limBDTmin = -2;//
  Float_t limBDTmax = 2;//
  Float_t gbdtBins[nBDT+1];
  Float_t bdtBins[nBDT+3];
  bdtBins[0] = limBDTmin;
  for(int i=0;i<=nBDT;i++)
    {
      Float_t iBDT = minBDT+i*widBDT;
      gbdtBins[i] = iBDT;
      bdtBins[i+1] = iBDT;
    }
  bdtBins[nBDT+2] = limBDTmax;

  TH1F* hcountEffS = new TH1F("hcountEffS",";BDT;Signal counts",nBDT+2,bdtBins);
  TH1F* hcountEffB = new TH1F("hcountEffB",";BDT;Background counts",nBDT+2,bdtBins);

  signal->Project("hcountEffS","BDT",Form("%s*(%s)",weight.Data(),cuts.Data()));
  background->Project("hcountEffB","BDT",Form("%s",cutb.Data()));
  TCanvas* chcountEffS = new TCanvas("chcountEffS","",600,600);
  hcountEffS->Draw();
  chcountEffS->SaveAs(Form("plots/%s_%s_pT_%.0f_%.0f_chcountEffS.pdf",MVAtype.Data(),colsyst.Data(),ptmin,ptmax));
  TCanvas* chcountEffB = new TCanvas("chcountEffB","",600,600);
  hcountEffB->Draw();
  chcountEffB->SaveAs(Form("plots/%s_%s_pT_%.0f_%.0f_chcountEffB.pdf",MVAtype.Data(),colsyst.Data(),ptmin,ptmax));
  
  TH1F* hdisEffS = (TH1F*)hcountEffS->Clone("hdisEffS");
  hdisEffS->Scale(1./hcountEffS->Integral());
  hdisEffS->SetLineColor(kRed);
  hdisEffS->SetFillStyle(3004);
  hdisEffS->SetFillColor(kRed);
  hdisEffS->SetLineWidth(3);
  hdisEffS->SetStats(0);
  TH1F* hdisEffB = (TH1F*)hcountEffB->Clone("hdisEffB");
  hdisEffB->Scale(1./hcountEffB->Integral());
  hdisEffB->SetLineColor(kBlue+1);
  hdisEffB->SetFillStyle(1001);
  hdisEffB->SetFillColor(kBlue-9);
  hdisEffB->SetLineWidth(3);
  hdisEffB->SetStats(0);

  TH2F* hemptydis = new TH2F("hemptydis","",50,-1.2,1.2,10,0.,hdisEffB->GetMaximum()*1.2);
  hemptydis->GetXaxis()->CenterTitle();
  hemptydis->GetYaxis()->CenterTitle();
  hemptydis->GetXaxis()->SetTitle("BDT");
  hemptydis->GetYaxis()->SetTitle("Probability");
  hemptydis->GetXaxis()->SetTitleOffset(0.9);
  hemptydis->GetYaxis()->SetTitleOffset(1.2);
  hemptydis->GetXaxis()->SetTitleSize(0.05);
  hemptydis->GetYaxis()->SetTitleSize(0.05);
  hemptydis->GetXaxis()->SetTitleFont(42);
  hemptydis->GetYaxis()->SetTitleFont(42);
  hemptydis->GetXaxis()->SetLabelFont(42);
  hemptydis->GetYaxis()->SetLabelFont(42);
  hemptydis->GetXaxis()->SetLabelSize(0.035);
  hemptydis->GetYaxis()->SetLabelSize(0.035);
  TCanvas* cdisEffSB = new TCanvas("cdisEffSB","",600,600);
  hemptydis->Draw();
  hdisEffB->Draw("same");
  hdisEffS->Draw("same");
  TLegend* leg = new TLegend(0.60,0.75,0.95,0.86);
  leg->SetBorderSize(0);
  leg->SetFillStyle(0);
  leg->AddEntry(hdisEffB,"Background","f");
  leg->AddEntry(hdisEffS,"Signal","f");
  leg->Draw("same");
  texPar->Draw();
  texPtY->Draw();
  cdisEffSB->SaveAs(Form("plots/%s_%s_pT_%.0f_%.0f_disEffSB.pdf",MVAtype.Data(),colsyst.Data(),ptmin,ptmax));

  Float_t allS=0,allB=0;
  Float_t effS[nBDT],effB[nBDT],effSig[nBDT];
  for(int i=0;i<nBDT;i++)
    {
      effS[i] = 0;
      effB[i] = 0;
    }
  for(int i=0;i<(nBDT+2);i++)
    {
      allS+=hcountEffS->GetBinContent(i+1);
      allB+=hcountEffB->GetBinContent(i+1);
      if(i==0) continue;
      for(int j=0;j<i;j++)
        {
          if(j<nBDT)
            {
              effS[j]+=hcountEffS->GetBinContent(i+1);
              effB[j]+=hcountEffB->GetBinContent(i+1);
            }
        }
    }
  for(int i=0;i<nBDT;i++)
    {
      effS[i]/=allS;
      effB[i]/=allB;
    }

  TH2F* hemptyeff = new TH2F("hemptyeff","",50,-1.2,1.2,10,0.,1.2);
  hemptyeff->GetXaxis()->CenterTitle();
  hemptyeff->GetYaxis()->CenterTitle();
  hemptyeff->GetXaxis()->SetTitle("BDT");
  hemptyeff->GetYaxis()->SetTitle("Efficiency");
  hemptyeff->GetXaxis()->SetTitleOffset(0.9);
  hemptyeff->GetYaxis()->SetTitleOffset(1.0);
  hemptyeff->GetXaxis()->SetTitleSize(0.05);
  hemptyeff->GetYaxis()->SetTitleSize(0.05);
  hemptyeff->GetXaxis()->SetTitleFont(42);
  hemptyeff->GetYaxis()->SetTitleFont(42);
  hemptyeff->GetXaxis()->SetLabelFont(42);
  hemptyeff->GetYaxis()->SetLabelFont(42);
  hemptyeff->GetXaxis()->SetLabelSize(0.035);
  hemptyeff->GetYaxis()->SetLabelSize(0.035);
  TGraph* geffS = new TGraph(nBDT,gbdtBins,effS);
  geffS->SetMarkerSize(1.1);
  geffS->SetMarkerColor(kRed);
  geffS->SetLineWidth(2);
  geffS->SetLineColor(kRed);
  TGraph* geffB = new TGraph(nBDT,gbdtBins,effB);
  geffB->SetMarkerSize(1.1);
  geffB->SetMarkerColor(kBlue+1);
  geffB->SetLineWidth(2);
  geffB->SetLineColor(kBlue+1);
  TCanvas* ceffSB = new TCanvas("ceffSB","",600,600);
  hemptyeff->Draw();
  texPar->Draw();
  texPtY->Draw();
  geffS->Draw("samepl");
  geffB->Draw("samepl");
  TLegend* legeff = new TLegend(0.60,0.75,0.95,0.86);
  legeff->SetBorderSize(0);
  legeff->SetFillStyle(0);
  legeff->AddEntry(geffB,"Background","pl");
  legeff->AddEntry(geffS,"Signal","pl");
  legeff->Draw("same");
  ceffSB->SaveAs(Form("plots/%s_%s_pT_%.0f_%.0f_EffvsBDT.pdf",MVAtype.Data(),colsyst.Data(),ptmin,ptmax));

  Float_t wSignal=0;
  Float_t wBackground=0;
  Float_t* weights = new Float_t[2];
  //
  calRatio(signal,background,generated,weights);//weight signal and background
  //

  wSignal = weights[1];
  wBackground = weights[0];

  cout<<endl;
  cout<<"Looking for max significance ..."<<endl;

  Double_t maxsig = wSignal*effS[0]/sqrt(wSignal*effS[0]+wBackground*effB[0]);
  int maxindex = 0;
  for(int i=0;i<nBDT;i++)
    {
      effSig[i] = wSignal*effS[i]/sqrt(wSignal*effS[i]+wBackground*effB[i]);
      if(effSig[i]>maxsig)
	{
	  maxsig=effSig[i];
	  maxindex=i;
	}
    }
  cout<<endl;
  cout<<" ╒══════════════════════════════════════════════════╕"<<endl;
  cout<<" |                     Opt Result                   |"<<endl;
  cout<<" ├────────────┬────────────┬───────────────┬────────┤"<<endl;
  cout<<" | "<<setiosflags(ios::left)<<setw(10)<<"Sig eff"<<" | "<<setiosflags(ios::left)<<setw(10)<<effS[maxindex]<<" | "<<setiosflags(ios::left)<<setw(13)<<"S/sqrt(S+B)"<<" | "<<setiosflags(ios::left)<<setw(6)<<maxsig<<" |"<<endl;
  cout<<" ├────────────┴────────────┴───┬───────────┴────────┤"<<endl;
  cout<<" | "<<setiosflags(ios::left)<<setw(27)<<"BDT cut value"<<" | "<<setiosflags(ios::left)<<setw(18)<<(minBDT+maxindex*widBDT)<<" |"<<endl;
  cout<<" ╘═════════════════════════════╧════════════════════╛"<<endl;
  cout<<endl;

  TH2F* hempty = new TH2F("hempty","",50,-1.2,1.2,10,0.,maxsig*1.2);  
  hempty->GetXaxis()->CenterTitle();
  hempty->GetYaxis()->CenterTitle();
  hempty->GetXaxis()->SetTitle("BDT");
  hempty->GetYaxis()->SetTitle("S/sqrt(S+B)");
  hempty->GetXaxis()->SetTitleOffset(0.9);
  hempty->GetYaxis()->SetTitleOffset(1.0);
  hempty->GetXaxis()->SetTitleSize(0.05);
  hempty->GetYaxis()->SetTitleSize(0.05);
  hempty->GetXaxis()->SetTitleFont(42);
  hempty->GetYaxis()->SetTitleFont(42);
  hempty->GetXaxis()->SetLabelFont(42);
  hempty->GetYaxis()->SetLabelFont(42);
  hempty->GetXaxis()->SetLabelSize(0.035);
  hempty->GetYaxis()->SetLabelSize(0.035);

  TGraph* gsig = new TGraph(nBDT,gbdtBins,effSig);
  TCanvas* csig = new TCanvas("csig","",600,600);
  hempty->Draw();
  texPar->Draw();
  texPtY->Draw();
  gsig->Draw("same*");
  csig->SaveAs(Form("plots/%s_%s_pT_%.0f_%.0f_Significance.pdf",MVAtype.Data(),colsyst.Data(),ptmin,ptmax));
}

void calRatio(TTree* signal, TTree* background, TTree* generated, Float_t* results, Bool_t verbose=false)
{
  TString sels = cuts;
  TString selb = cutb;
  TString selg = cutg;

  //Background candidate number
  TH1D* hmassB = new TH1D("hmassB",";B mass (GeV/c^{2});Background Entries",50,0,10);
  background->Project("hmassB","Bmass",selb);
  TCanvas* cmassB = new TCanvas("cmassB","",600,600);
  hmassB->Draw();
  cmassB->SaveAs(Form("plots/%s_%s_pT_%.0f_%.0f_Background.pdf",MVAtype.Data(),colsyst.Data(),ptmin,ptmax));
  Int_t nentriesB = hmassB->Integral();

  //FONLL
  //
  ifstream getdata("fonlls/fo_pp_Bplus_5p03TeV_y2p4.dat");
  if(!getdata.is_open()) cout<<"Opening the file fails"<<endl;
  Float_t tem;
  Int_t nbin=0;
  while (!getdata.eof())
    {
      getdata>>pt[nbin]>>central[nbin]>>tem>>tem>>tem>>tem>>tem>>tem>>tem>>tem;
      if(pt[nbin]>=ptmin&&pt[nbin]<=ptmax) nbin++;
    }
  TH1D* hfonll = new TH1D("hfonll",";B p_{T} (GeV/c);FONLL differential xsection",nbin-1,pt);
  for(int i=0;i<nbin;i++)
    {
      hfonll->SetBinContent(i,central[i]);
    }
  TCanvas* cfonll = new TCanvas("cfonll","",600,600);
  hfonll->Draw();
  cfonll->SaveAs(Form("plots/%s_%s_pT_%.0f_%.0f_Fonll.pdf",MVAtype.Data(),colsyst.Data(),ptmin,ptmax));
  //

  TH1D* hrec = new TH1D("hrec",";B p_{T} (GeV/c);Signal reco entries",nbin-1,pt);
  TH1D* hgen = new TH1D("hgen",";B p_{T} (GeV/c);Generated entries",nbin-1,pt);
  TH1D* heff = new TH1D("heff",";B p_{T} (GeV/c);Prefilter efficiency",nbin-1,pt);
  if(isPbPb) signal->Project("hrec","Bpt",Form("%s*(%s&&hiBin>=0&&hiBin<=200)",weight.Data(),mycuts.Data()));
  else signal->Project("hrec","Bpt",Form("%s*(%s)",weight.Data(),mycuts.Data()));
  if(isPbPb) generated->Project("hgen","Gpt",Form("%s*(%s&&hiBin>=0&&hiBin<=200)",weight.Data(),mycutg.Data()));
  else generated->Project("hgen","Gpt",Form("%s*(%s)",weight.Data(),mycutg.Data()));
  heff->Divide(hrec,hgen,1.,1.,"B");
  TCanvas* ceff = new TCanvas("ceff","",600,600);
  heff->Draw();
  ceff->SaveAs(Form("plots/%s_%s_pT_%.0f_%.0f_EffPrefilter.pdf",MVAtype.Data(),colsyst.Data(),ptmin,ptmax));

  TH1D* htheoryreco = new TH1D("htheoryreco","",nbin-1,pt);
  htheoryreco->Multiply(heff,hfonll,1,1,"B");

  //use lumi//
  Double_t lumi = isPbPb?15.17:27.7;
  Double_t BR = 6.09604e-5;
  Double_t deltapt = 0.25;
  //central[i] - in pb/GeV/c

  Double_t yieldDzero = htheoryreco->Integral();
  yieldDzero*=BR*deltapt*lumi*raa;//
  //

  results[0] = nentriesB*0.08/0.1;
  results[1] = yieldDzero;
  cout<<endl;
  cout<<" ╒══════════════════════════════════════════════════╕"<<endl;
  cout<<" |                   Weight Result                  |"<<endl;
  cout<<" ├────────────┬────────────┬────────────┬───────────┤"<<endl;
  cout<<" | "<<setiosflags(ios::left)<<setw(10)<<"Bkg #"<<" | "<<setiosflags(ios::left)<<setw(10)<<nentriesB<<" | "<<setiosflags(ios::left)<<setw(10)<<"Sig reg"<<" | "<<setiosflags(ios::left)<<setw(9)<<setprecision(3)<<0.16<<" |"<<endl;
  cout<<" ├────────────┼────────────┼────────────┼───────────┤"<<endl;
  cout<<" | "<<setiosflags(ios::left)<<setw(10)<<"SigWeight"<<" | "<<setiosflags(ios::left)<<setw(10)<<yieldDzero<<" | "<<setiosflags(ios::left)<<setw(10)<<"BkgWeight"<<" | "<<setiosflags(ios::left)<<setw(9)<<nentriesB*0.08/0.1<<" |"<<endl;
  cout<<" ╘════════════╧════════════╧════════════╧═══════════╛"<<endl;
}
