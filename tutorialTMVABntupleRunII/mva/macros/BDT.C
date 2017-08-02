#include "../../includes/uti.h"
#include "../header.h"
#define MAX_XB       100000

void BDT(TString inputname, TString outputname,
         Float_t ptmin, Float_t ptmax, TString mvatype)
{
  void makeoutput(TString infname, TString ofname, TString treename);
  makeoutput(inputname,Form("%s.root",outputname.Data()),Form("%s_pt_%.0f_%.0f",mvatype.Data(),ptmin,ptmax));
}

void makeoutput(TString infname, TString ofname, TString treename)
{
   TFile* inf = new TFile(infname);
   TTree* t = (TTree*)inf->Get("ntDkpi");
   
   Int_t           Dsize;
   Float_t         Dchi2cl[MAX_XB];   //[Bsize]
   Float_t         DsvpvDistance[MAX_XB];   //[Bsize]
   Float_t         DsvpvDisErr[MAX_XB];   //[Bsize]
   Float_t         Dalpha[MAX_XB];   //[Bsize]   

   t->SetBranchAddress("Dsize", &Dsize);
   t->SetBranchAddress("Dalpha", Dalpha);
   t->SetBranchAddress("Dchi2cl", Dchi2cl);
   t->SetBranchAddress("DsvpvDistance", DsvpvDistance);
   t->SetBranchAddress("DsvpvDisErr", DsvpvDisErr);
 
   std::vector<std::string> theInputVars;
   std::string a1 = "Dalpha";
   std::string a2 = "DsvpvDistance/DsvpvDisErr";
   std::string a3 = "Dchi2cl";
   theInputVars.push_back(a1);
   theInputVars.push_back(a2);
   theInputVars.push_back(a3);

   std::vector<double> inputValues;
   ReadBDT mva(theInputVars);
  
   TFile* outf = new TFile(ofname,"recreate");
   TTree* mvaTree = new TTree(Form("%s",treename.Data()),"MVA");

   double MVA[MAX_XB];
   mvaTree->Branch("Dsize",&Dsize,"Dsize/I");
   mvaTree->Branch(Form("%s",treename.Data()),MVA,Form("%s[Dsize]/D",treename.Data()));
   std::cout<<std::endl;
   std::cout<<"  Input file: "<<infname<<std::endl;
   std::cout<<"  Calculating MVA values ..."<<std::endl;
   for(int i=0;i<t->GetEntries();i++)
     {
       if(i%10000==0) std::cout<<std::setiosflags(std::ios::left)<<"  [ \033[1;36m"<<std::setw(10)<<i<<"\033[0m"<<" / "<<std::setw(10)<<t->GetEntries()<<" ] "<<"\033[1;36m"<<Form("%.0f",100.*i/t->GetEntries())<<"%\033[0m"<<"\r"<<std::flush;
       t->GetEntry(i);
       for(int j=0;j<Dsize;j++)
         {
           inputValues.clear();
           inputValues.push_back(Dalpha[j]);
           inputValues.push_back(DsvpvDistance[j]/DsvpvDisErr[j]);
		          inputValues.push_back(Dchi2cl[j]);
           MVA[j] = mva.GetMvaValue(inputValues);      
         }
      mvaTree->Fill();
   }
   std::cout<<std::endl<<"  Processed "<<"\033[1;31m"<<t->GetEntries()<<"\033[0m event(s)."<<std::endl;
   outf->cd();
   outf->Write();
   outf->Close();
}

int main(int argc, char* argv[])
{
  if(argc==6)
    {
      BDT(argv[1],argv[2],atof(argv[3]),atof(argv[4]),argv[5]);
      return 0;
    }
  else
    {
      std::cout<<"  Error: invalid argument number - BDT()"<<std::endl;
      return 1;
    }
}
