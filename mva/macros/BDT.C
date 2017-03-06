#include "../header.h"
#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <vector>
#include <iostream>
#include "../../prefilter.h"

void BDT(int pbpb, float ptMin , float ptMax, string mvatype)
{
  void makeoutput(TString infname, TString ofname, TString mvatype);

  MVAtype = (TString)mvatype;
  isPbPb = (bool)pbpb;
  ptmin = ptMin;
  ptmax = ptMax;
  if(isPbPb)
    {
      inputSname = inputSname_PP;
      inputBname = inputBname_PP;
      colsyst = "PbPb";
    }
  else{
    inputSname = inputSname_pp;
    inputBname = inputBname_pp;
    colsyst = "pp";
  }
  
  TString outfile1 =Form("../MVAfiles/%s_%s_%.0f_%.0f_DATA.root",MVAtype.Data(),colsyst.Data(),ptMin,ptMax) ;
  makeoutput(inputBname,outfile1,MVAtype);
  TString outfile2 =Form("../MVAfiles/%s_%s_%.0f_%.0f_MC.root",MVAtype.Data(),colsyst.Data(),ptMin,ptMax) ;
  makeoutput(inputSname,outfile2,MVAtype);
}

void makeoutput(TString infname, TString ofname, TString mvatype)
{
   TFile *inf = new TFile(infname);
   TTree *t = (TTree*)inf->Get("ntKp");
   
   Int_t           Bsize;
   Float_t         Btrk1Pt[1000];   //[Bsize]
   Float_t         Bchi2cl[1000];   //[Bsize]
   Float_t         BsvpvDistance[1000];   //[Bsize]
   Float_t         BsvpvDisErr[1000];   //[Bsize]
   Float_t         Bdtheta[1000];   //[Bsize]
   
   Float_t         Btrk1Eta[1000];
   Float_t         Bmumupt[1000];

   t->SetBranchAddress("Bsize", &Bsize);
   t->SetBranchAddress("Btrk1Pt", Btrk1Pt);
   t->SetBranchAddress("Bchi2cl", Bchi2cl);
   t->SetBranchAddress("BsvpvDistance", BsvpvDistance);
   t->SetBranchAddress("BsvpvDisErr", BsvpvDisErr);
   t->SetBranchAddress("Bdtheta", Bdtheta);

   t->SetBranchAddress("Btrk1Eta", Btrk1Eta);
   t->SetBranchAddress("Bmumupt", Bmumupt);
   
   std::vector<std::string> theInputVars;
   string a1="Btrk1Pt";
   string a2="Bchi2cl";
   string a3="BsvpvDistance/BsvpvDisErr";
   string a4="cos(Bdtheta)";
   string a5="abs(Btrk1Eta)";
   string a6="Bmumupt";
   theInputVars.push_back(a1);
   theInputVars.push_back(a2);
   theInputVars.push_back(a3);
   theInputVars.push_back(a4); 
   theInputVars.push_back(a5); 
   theInputVars.push_back(a6); 

   vector<double> inputValues;
//cout<<"kf1500"<<endl;
   ReadBDT mva(theInputVars);
//cout<<"kf1600"<<endl;
  
   TFile *outf = new TFile(ofname,"recreate");
   TTree *mvaTree = new TTree("mvaTree","MVA");

   double MVA[1000];
   mvaTree->Branch("Bsize",&Bsize,"Bsize/I");
   mvaTree->Branch(mvatype.Data(),MVA,Form("%s[Bsize]/D",mvatype.Data()));
   
   for(int i=0;i<t->GetEntries();i++)
     {
       t->GetEntry(i);
       if(i%1000000==0) cout <<i<<" / "<<t->GetEntries()<<endl;
       for(int j=0;j<Bsize;j++)
         {
           inputValues.clear();
           inputValues.push_back(Btrk1Pt[j]);
           inputValues.push_back(Bchi2cl[j]);
           inputValues.push_back(BsvpvDistance[j]/BsvpvDisErr[j]);
           inputValues.push_back(cos(Bdtheta[j]));
           inputValues.push_back(abs(Btrk1Eta[j]));
           inputValues.push_back(Bmumupt[j]);
           MVA[j]=mva.GetMvaValue(inputValues);      
         }
      mvaTree->Fill();
   }
   outf->cd();
   outf->Write();
   outf->Close();
}
