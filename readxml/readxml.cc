using namespace std;
#include "uti.h"
#include "readxml.h"
#include "Tools.h"
#include "../prefilter.h"

TString cutss;
TString cutsb;
TString cutsg;
TString weight="1";

//Raa=0.49,0.44,0.42,0.41,0.41
void readxml(Int_t pbpb=1, TString mva="CutsSA", Float_t ptMin=30., Float_t ptMax=50., Float_t RAA=1.)
{
  gStyle->SetTextSize(0.05);
  gStyle->SetTextFont(42);
  gStyle->SetPadRightMargin(0.043);
  gStyle->SetPadLeftMargin(0.18);
  gStyle->SetPadTopMargin(0.1);
  gStyle->SetPadBottomMargin(0.145);
  gStyle->SetTitleX(.0f);

  void calRatio(TTree* signal, TTree* background, TTree* generated, Float_t* results, Bool_t verbose=false);
  isPbPb = (bool)pbpb;
  MVAtype = (TString)mva;
  ptmin = ptMin;
  ptmax = ptMax;

  inputSname = isPbPb?inputSname_PP:inputSname_pp;
  inputBname = isPbPb?inputBname_PP:inputBname_pp;      
  raa = isPbPb?RAA:1.;
  mycuts = isPbPb?mycuts_PP:mycuts_pp;
  mycutb = isPbPb?mycutb_PP:mycutb_pp;
  mycutg = isPbPb?mycutg_PP:mycutg_pp;
  colsyst = isPbPb?"PbPb":"pp";

  cutss = isPbPb?Form("(%s)&&Bpt>%f&&Bpt<%f&&hiBin>=0&&hiBin<=200",mycuts.Data(),ptmin,ptmax):Form("(%s)&&Bpt>%f&&Bpt<%f",mycuts.Data(),ptmin,ptmax);
  cutsb = isPbPb?Form("(%s)&&Bpt>%f&&Bpt<%f&&hiBin>=0&&hiBin<=200",mycutb.Data(),ptmin,ptmax):Form("(%s)&&Bpt>%f&&Bpt<%f",mycutb.Data(),ptmin,ptmax);
  cutsg = isPbPb?Form("(%s)&&Gpt>%f&&Gpt<%f&&hiBin>=0&&hiBin<=200",mycutg.Data(),ptmin,ptmax):Form("(%s)&&Gpt>%f&&Gpt<%f",mycutg.Data(),ptmin,ptmax);

  TFile *inputS = new TFile(inputSname.Data());
  TFile *inputB = new TFile(inputBname.Data());

  TTree *background = (TTree*)inputB->Get("ntKp");
  background->AddFriend("ntHlt");
  background->AddFriend("ntHi");
  background->AddFriend("ntSkim");

  TTree *generated = (TTree*)inputS->Get("ntGen");
  generated->AddFriend("ntHlt");
  generated->AddFriend("ntHi");

  TTree *signal = (TTree*)inputS->Get("ntKp");
  signal->AddFriend("ntHlt");
  signal->AddFriend("ntHi");
  signal->AddFriend("ntSkim");

  //read weight file
  TString fileName = Form("../myTMVA/weights/TMVA_%s_%s_%.0f_%.0f.weights.xml",MVAtype.Data(),colsyst.Data(),ptmin,ptmax);
  const char* filename = fileName.Data();
  void *doc = TMVA::gTools().xmlengine().ParseFile(filename,TMVA::gTools().xmlenginebuffersize());
  void* rootnode = TMVA::gTools().xmlengine().DocGetRootElement(doc); // node "MethodSetup"
  TString fullMethodName(""); 
  TMVA::gTools().ReadAttr(rootnode, "Method", fullMethodName);

  cout<<" ╒══════════════════════════════════════════════════╕"<<endl;
  cout<<" |               Cut Opt Configuration              |"<<endl;
  cout<<" ├────────────┬────────────────────────────┬────────┤"<<endl;
  cout<<" | "<<setiosflags(ios::left)<<setw(10)<<"Method"<<" | "<<setiosflags(ios::left)<<setw(26)<<fullMethodName<<" | "<<setiosflags(ios::left)<<setw(6)<<" "<<" |"<<endl;

  void *opts = TMVA::gTools().GetChild(rootnode,"Options");
  void* opt = TMVA::gTools().GetChild(opts,"Option");

  TString varProp("");
  while (opt)
    {
      TString optname("");
      TMVA::gTools().ReadAttr(opt, "name", optname);
      if (optname=="VarProp") varProp = TMVA::gTools().GetContent(opt);
      opt = TMVA::gTools().GetNextChild(opt);
    }

  TObjArray *marginclass = varProp.Tokenize(" ");
  std::vector<TString> margins;//avoid objarrays
  for(int i=0;i<marginclass->GetEntries();i++)
    {
      margins.push_back(((TObjString *)(marginclass->At(i)))->String());
    }
  void* variables = TMVA::gTools().GetChild(rootnode,"Variables");
  unsigned int nVar=0;
  std::vector<TString> varnames;
  TMVA::gTools().ReadAttr(variables, "NVar", nVar);

  void* var = TMVA::gTools().GetChild(variables,"Variable");
  for(unsigned int k=0;k<nVar;k++)
    {
      TString varname("");
      TMVA::gTools().ReadAttr(var, "Expression", varname);
      TString tem = Form("Variable%i",k);
      varval[k] = varname;
      cout<<" ├────────────┼────────────────────────────┼────────┤"<<endl;
      cout<<" | "<<setiosflags(ios::left)<<setw(10)<<tem<<" | "<<setiosflags(ios::left)<<setw(26)<<varname<<" | "<<setiosflags(ios::left)<<setw(6)<<margins[k]<<" |"<<endl;
      var = TMVA::gTools().GetNextChild(var);
      varnames.push_back(varname);
    }
  cout<<" ╞════════════╪════════════════════════════╪════════╡"<<endl;
  TString ptstring = Form("(%.1f,%.1f)",ptmin,ptmax);
  cout<<" | "<<setiosflags(ios::left)<<setw(10)<<"Pt"<<" | "<<setiosflags(ios::left)<<setw(26)<<ptstring<<" | "<<setiosflags(ios::left)<<setw(6)<<" "<<" |"<<endl;
  cout<<" ├────────────┼────────────────────────────┼────────┤"<<endl;
  cout<<" | "<<setiosflags(ios::left)<<setw(10)<<"Raa"<<" | "<<setiosflags(ios::left)<<setw(26)<<raa<<" | "<<setiosflags(ios::left)<<setw(6)<<" "<<" |"<<endl;
  cout<<" ╘════════════╧════════════════════════════╧════════╛"<<endl;
  cout<<endl;


  void* weight = TMVA::gTools().GetChild(rootnode,"Weights");
  void* eff = TMVA::gTools().GetChild(weight,"Bin");
  int n=0;
  while(eff)
    {
      TMVA::gTools().ReadAttr(eff, "effS", effS[n]);
      TMVA::gTools().ReadAttr(eff, "effB", effB[n]);
      void* cutsnode = TMVA::gTools().GetChild(eff,"Cuts");
      TString cut;
      for(unsigned long l=0;l<varnames.size();l++)
	{
	  double min,max;
	  TMVA::gTools().ReadAttr(cutsnode, TString("cutMin_")+l, min);
	  TMVA::gTools().ReadAttr(cutsnode, TString("cutMax_")+l, max);
	  TString lessmax = "<"; lessmax+=max;
	  TString moremin = ">"; moremin+=min;
	  if(margins[l]=="FMin")
	    {
	      cut+=" && "+varnames[l]+lessmax;
	      cutval[l].push_back(max);
	    }
	  if(margins[l]=="FMax")
	    {
	      cut+=" && "+varnames[l]+moremin;
	      cutval[l].push_back(min);
	    }
	}
      eff = TMVA::gTools().GetNextChild(eff);
      n++;
    }
  TMVA::gTools().xmlengine().FreeDoc(doc);

  float wSignal=0;
  float wBackground=0;
  float* weights = new float[2];
  //
  calRatio(signal,background,generated,weights);//weight signal and background
  //

  wSignal = weights[1];
  wBackground = weights[0];

  cout<<endl;
  cout<<"Looking for max significance ..."<<endl;

  double maxsig = wSignal*effS[1]/sqrt(wSignal*effS[1]+wBackground*effB[1]);
  int maxindex = 1;
  effS[0]=0;
  for(int i=1;i<100;i++)
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

  for(unsigned int m=0;m<nVar;m++)
    {
      if(m) cout<<" ├─────────────────────────────┼────────────────────┤"<<endl;
      cout<<" | "<<setiosflags(ios::left)<<setw(27)<<varval[m]<<" | "<<setiosflags(ios::left)<<setw(18)<<cutval[m].at(maxindex)<<" |"<<endl;
    }
  cout<<" ╘═════════════════════════════╧════════════════════╛"<<endl;
  cout<<endl;

  TH2F* hempty = new TH2F("hempty","",50,0,1.,10,0.,maxsig*1.2);  
  hempty->GetXaxis()->CenterTitle();
  hempty->GetYaxis()->CenterTitle();
  hempty->GetXaxis()->SetTitle("Signal efficiency");
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
  TLatex* texPar = new TLatex(0.18,0.93, Form("%s 5.02 TeV B^{+}",colsyst.Data()));
  texPar->SetNDC();
  texPar->SetTextAlign(12);
  texPar->SetTextSize(0.04);
  texPar->SetTextFont(42);
  TLatex* texPtY = new TLatex(0.96,0.93, Form("|y|<2.4, %.1f<p_{T}<%.1f GeV/c",ptmin,ptmax));
  texPtY->SetNDC();
  texPtY->SetTextAlign(32);
  texPtY->SetTextSize(0.04);
  texPtY->SetTextFont(42);

  TGraph* gsig = new TGraph(100,effS,effSig);
  TCanvas* csig = new TCanvas("csig","",600,600);
  hempty->Draw();
  texPar->Draw();
  texPtY->Draw();
  gsig->Draw("same*");
  csig->SaveAs(Form("plots/Significance_%s_%s_%.0f_%.0f.pdf",MVAtype.Data(),colsyst.Data(),ptmin,ptmax));

}

void calRatio(TTree* signal, TTree* background, TTree* generated, Float_t* results, Bool_t verbose=false)
{
  TString sels = cutss;
  TString selb = cutsb;
  TString selg = cutsg;

  //Background candidate number
  TH1D* hmassB = new TH1D("hmassB",";B mass (GeV/c^{2});Background Entries",50,0,10);
  background->Project("hmassB","Bmass",selb);
  TCanvas* cmassB = new TCanvas("cmassB","",600,600);
  hmassB->Draw();
  cmassB->SaveAs(Form("plots/Background_%s_%s_%.0f_%.0f.pdf",MVAtype.Data(),colsyst.Data(),ptmin,ptmax));
  int nentriesB = hmassB->Integral();

  //FONLL
  ifstream getdata("fonlls/fo_pp_Bplus_5p03TeV_y2p4.dat");
  if(!getdata.is_open()) cout<<"Opening the file fails"<<endl;
  float tem;
  int nbin=0;
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
  cfonll->SaveAs(Form("plots/Fonll_%s_%s_%.0f_%.0f.pdf",MVAtype.Data(),colsyst.Data(),ptmin,ptmax));

  TH1D* hrec = new TH1D("hrec",";B p_{T} (GeV/c);Signal reco entries",nbin-1,pt);
  TH1D* hgen = new TH1D("hgen",";B p_{T} (GeV/c);Generated entries",nbin-1,pt);
  TH1D* heff = new TH1D("heff",";B p_{T} (GeV/c);Prefilter efficiency",nbin-1,pt);
  signal->Project("hrec","Bpt",Form("%s*(%s)",weight.Data(),sels.Data()));
  generated->Project("hgen","Gpt",Form("%s*(%s)",weight.Data(),selg.Data()));
  heff->Divide(hrec,hgen,1.,1.,"B");
  TCanvas* ceff = new TCanvas("ceff","",600,600);
  heff->Draw();
  ceff->SaveAs(Form("plots/EffPrefilter_%s_%s_%.0f_%.0f.pdf",MVAtype.Data(),colsyst.Data(),ptmin,ptmax));

  TH1D* htheoryreco = new TH1D("htheoryreco","",nbin-1,pt);
  htheoryreco->Multiply(heff,hfonll,1,1,"B");

  Double_t lumi = isPbPb?15.17:27.7;
  double BR = 6.09604e-5;
  double deltapt = 0.25;
  //central[i] - in pb/GeV/c

  double yieldDzero = htheoryreco->Integral();
  yieldDzero*=BR*deltapt*lumi*raa*2;

  results[0] = nentriesB*0.08/0.1;//0.05: half of sideband width
  //results[0] = nentriesB*Nsigma*sigma/0.05;//0.05: half of sideband width
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
