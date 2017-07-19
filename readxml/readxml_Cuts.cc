#include "readxml.h"

void readxml_Cuts(TString inputSname, TString inputBname, TString outputname,
                  TString weightfile, TString collisionsyst,
                  TString mycuts, TString mycutb, TString mycutg, TString weight,
                  TString mvatype, Float_t ptmin, Float_t ptmax,
                  Float_t lumi, Float_t raa=1.)
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

  Bool_t isPbPb = collisionsyst=="PbPb"?true:false;
  std::ofstream ofresult(Form("%s_%s.txt",outputresult.Data(),outputname.Data()),std::ofstream::out);
  cfout cfresult(ofresult, std::cout);

  TString cuts = isPbPb?Form("(%s)&&Bpt>%f&&Bpt<%f&&hiBin>=0&&hiBin<=200",mycuts.Data(),ptmin,ptmax):Form("(%s)&&Bpt>%f&&Bpt<%f",mycuts.Data(),ptmin,ptmax);
  TString cutb = isPbPb?Form("(%s)&&Bpt>%f&&Bpt<%f&&hiBin>=0&&hiBin<=200",mycutb.Data(),ptmin,ptmax):Form("(%s)&&Bpt>%f&&Bpt<%f",mycutb.Data(),ptmin,ptmax);
  TString cutg = isPbPb?Form("(%s)&&Gpt>%f&&Gpt<%f&&hiBin>=0&&hiBin<=200",mycutg.Data(),ptmin,ptmax):Form("(%s)&&Gpt>%f&&Gpt<%f",mycutg.Data(),ptmin,ptmax);

  // read weight file
  const char* filename = weightfile;
  void *doc = TMVA::gTools().xmlengine().ParseFile(filename,TMVA::gTools().xmlenginebuffersize());
  void* rootnode = TMVA::gTools().xmlengine().DocGetRootElement(doc); //node "MethodSetup"
  TString fullMethodName("");
  TMVA::gTools().ReadAttr(rootnode, "Method", fullMethodName);

  cfresult<<std::endl;
  cfresult<<"  ===================================================="<<std::endl;
  cfresult<<" |                Cut Opt Configuration               |"<<std::endl;
  cfresult<<"  ----------------------------------------------------"<<std::endl;
  cfresult<<" | "<<std::left<<std::setw(10)<<"Method"<<" | "<<std::setw(28)<<fullMethodName<<" | "<<std::setw(6)<<" "<<" |"<<std::endl;

  void* opts = TMVA::gTools().GetChild(rootnode,"Options");
  void* opt = TMVA::gTools().GetChild(opts,"Option");

  TString varProp("");
  while(opt)
    {
      TString optname("");
      TMVA::gTools().ReadAttr(opt, "name", optname);
      if(optname=="VarProp") varProp = TMVA::gTools().GetContent(opt);
      opt = TMVA::gTools().GetNextChild(opt);
    }

  // variables
  TObjArray *marginclass = varProp.Tokenize(" ");
  std::vector<TString> margins; //avoid objarrays
  for(int i=0;i<marginclass->GetEntries();i++)
    {
      margins.push_back(((TObjString *)(marginclass->At(i)))->String());
    }
  void* variables = TMVA::gTools().GetChild(rootnode,"Variables");
  UInt_t nVar=0;
  std::vector<TString> varnames;
  std::vector<double> varmins[NEff];
  std::vector<double> varmaxs[NEff];
  TMVA::gTools().ReadAttr(variables, "NVar", nVar);

  void* var = TMVA::gTools().GetChild(variables,"Variable");
  for(unsigned int k=0;k<nVar;k++)
    {
      TString varname("");
      TMVA::gTools().ReadAttr(var, "Expression", varname);
      TString tem = Form("Variable%i",k);
      cfresult<<"  ----------------------------------------------------"<<std::endl;
      cfresult<<" | "<<std::left<<std::setw(10)<<tem<<" | "<<std::setw(28)<<varname<<" | "<<std::setw(6)<<margins[k]<<" |"<<std::endl;
      var = TMVA::gTools().GetNextChild(var);
      varnames.push_back(varname);
    }
  cfresult<<"  ===================================================="<<std::endl;
  std::cout<<std::endl;

  // prepare efficiency
  void* weightsnode = TMVA::gTools().GetChild(rootnode,"Weights");
  void* eff = TMVA::gTools().GetChild(weightsnode,"Bin");
  Double_t effS[NEff], effB[NEff], effSig[NEff];
  int n=0;
  while(eff)
    {
      TMVA::gTools().ReadAttr(eff, "effS", effS[n]);
      TMVA::gTools().ReadAttr(eff, "effB", effB[n]);
      void* cutsnode = TMVA::gTools().GetChild(eff,"Cuts");
      for(ULong_t l=0;l<varnames.size();l++)
        {
          Double_t vmin,vmax;
          TMVA::gTools().ReadAttr(cutsnode, TString("cutMin_")+l, vmin);
          TMVA::gTools().ReadAttr(cutsnode, TString("cutMax_")+l, vmax);
          varmins[n].push_back(vmin);
          varmaxs[n].push_back(vmax);
        }
      eff = TMVA::gTools().GetNextChild(eff);
      n++;
    }
  TMVA::gTools().xmlengine().FreeDoc(doc);

  //
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

  Float_t wSignal=0, wBackground=0;
  Float_t* weights = new Float_t[2];
  //
  calRatio(signal, background, generated, cuts, cutb, cutg, weight, ptmin, ptmax, "fonlls/fo_pp_Bplus_5p03TeV_y2p4.dat", lumi, raa, outputname, weights);
  //

  wSignal = weights[1];
  wBackground = weights[0];

  std::cout<<std::endl;
  std::cout<<"Looking for max significance ..."<<std::endl;

  Double_t maxsig = 0;
  Int_t maxindex = 0;
  for(int i=0;i<NEff;i++)
    {
      effSig[i] = wSignal*effS[i]/sqrt(wSignal*effS[i]+wBackground*effB[i]);
      if(effSig[i]>maxsig)
        {
          maxsig=effSig[i];
          maxindex=i;
        }
    }

  cfresult<<std::endl;
  cfresult<<"  =========================================================="<<std::endl;
  cfresult<<" |                         Opt Result                       |"<<std::endl;
  cfresult<<"  ----------------------------------------------------------"<<std::endl;
  cfresult<<" | "<<std::setiosflags(std::ios::left)<<std::setw(15)<<"pT"<<" | "<<std::setw(38)<<Form("%.1f - %.1f GeV/c",ptmin,ptmax)<<" |"<<std::endl;
  cfresult<<"  ----------------------------------------------------------"<<std::endl;
  cfresult<<" | "<<std::setiosflags(std::ios::left)<<std::setw(15)<<"RAA"<<" | "<<std::setw(38)<<raa<<" |"<<std::endl;
  cfresult<<"  ----------------------------------------------------------"<<std::endl;
  cfresult<<" | "<<std::setiosflags(std::ios::left)<<std::setw(15)<<"Sig reg"<<" | "<<std::setw(9)<<Form("+- %.2f",dmassBsignal)<<" | "<<std::setw(13)<<"Sideband bkg"<<" | "<<std::setw(10)<<Form("%.0f",wBackground*(dmassBsidbandH-dmassBsidbandL)/dmassBsignal)<<" |"<<std::endl;
  cfresult<<"  ----------------------------------------------------------"<<std::endl;
  cfresult<<" | "<<std::setiosflags(std::ios::left)<<std::setw(15)<<"weight S"<<" | "<<std::setw(9)<<Form("%.0f",wSignal)<<" | "<<std::setw(13)<<"weight B"<<" | "<<std::setw(10)<<Form("%.0f",wBackground)<<" |"<<std::endl;
  cfresult<<"  ----------------------------------------------------------"<<std::endl;
  cfresult<<" | "<<std::setiosflags(std::ios::left)<<std::setw(15)<<"S"<<" | "<<std::setw(9)<<Form("%.0f",wSignal*effS[maxindex])<<" | "<<std::setw(13)<<"B"<<" | "<<std::setw(10)<<Form("%.0f",wBackground*effB[maxindex])<<" |"<<std::endl;
  cfresult<<"  ----------------------------------------------------------"<<std::endl;
  cfresult<<" | "<<std::setiosflags(std::ios::left)<<std::setw(15)<<"effS"<<" | "<<std::setw(9)<<effS[maxindex]<<" | "<<std::setw(13)<<"sig"<<" | "<<std::setw(10)<<Form("%.2f",effSig[maxindex])<<" |"<<std::endl;
  cfresult<<"  ----------------------------------------------------------"<<std::endl;
  for(unsigned int m=0;m<nVar;m++)
    {
      if(m) cfresult<<"  ----------------------------------------------------------"<<std::endl;
      cfresult<<" | "<<std::left<<std::setw(35)<<varnames.at(m)<<" | "<<std::setw(18)<<varmins[maxindex].at(m)<<" |"<<std::endl;
      cfresult<<"  ----------------------------------------------------------"<<std::endl;
      cfresult<<" | "<<std::left<<std::setw(35)<<" "<<" | "<<std::setw(18)<<varmaxs[maxindex].at(m)<<" |"<<std::endl;
    }
  cfresult<<"  =========================================================="<<std::endl;
  cfresult<<std::endl;

  ofresult.close();

  TH2F* hempty = new TH2F("hempty",Form(";%s;S/sqrt(S+B)",mvatype.Data()),50,0,1,10,0.,maxsig*1.4);
  sethempty(hempty);
  TLatex* texPt = new TLatex(0.22,0.823, Form("%.0f < B^{+} p_{T} < %.0f GeV",ptmin,ptmax));
  texPt->SetNDC();
  texPt->SetTextAlign(12);
  texPt->SetTextSize(0.045);
  texPt->SetTextFont(42);
  TLatex* texY = new TLatex(0.22,0.773, "|y| < 2.4");
  texY->SetNDC();
  texY->SetTextAlign(12);
  texY->SetTextSize(0.045);
  texY->SetTextFont(42);
  TGraph* gsig = new TGraph(NEff,effS,effSig);
  TCanvas* csig = new TCanvas("csig","",600,600);
  hempty->Draw();
  gsig->Draw("same*");
  drawCMS(collisionsyst);
  texPt->Draw();
  texY->Draw();
  csig->SaveAs(Form("%s_%s.pdf",outputsignificance.Data(),outputname.Data()));

}

int main(int argc, char* argv[])
{
  if(argc==15)
    {
      readxml_Cuts(argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], argv[7], argv[8], argv[9], argv[10], atof(argv[11]), atof(argv[12]), atof(argv[13]), atof(argv[14]));
      return 0;
    }
  if(argc==14)
    {
      readxml_Cuts(argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], argv[7], argv[8], argv[9], argv[10], atof(argv[11]), atof(argv[12]), atof(argv[13]));
      return 0;
    }
  else
    {
      std::cout<<"  Error: invalid argument number - readxml_Cuts()"<<std::endl;
      return 1;
    }
}
