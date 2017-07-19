#include "readxml.h"

void readxml(TString inputSname, TString inputBname, TString outputname,
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

  TString cuts = isPbPb?Form("(%s)&&Bpt>%f&&Bpt<%f&&hiBin>=0&&hiBin<=200",mycuts.Data(),ptmin,ptmax):Form("(%s)&&Bpt>%f&&Bpt<%f",mycuts.Data(),ptmin,ptmax);
  TString cutb = isPbPb?Form("(%s)&&Bpt>%f&&Bpt<%f&&hiBin>=0&&hiBin<=200",mycutb.Data(),ptmin,ptmax):Form("(%s)&&Bpt>%f&&Bpt<%f",mycutb.Data(),ptmin,ptmax);
  TString cutg = isPbPb?Form("(%s)&&Gpt>%f&&Gpt<%f&&hiBin>=0&&hiBin<=200",mycutg.Data(),ptmin,ptmax):Form("(%s)&&Gpt>%f&&Gpt<%f",mycutg.Data(),ptmin,ptmax);

  // read weight file
  const char* filename = weightfile;
  void *doc = TMVA::gTools().xmlengine().ParseFile(filename,TMVA::gTools().xmlenginebuffersize());
  void* rootnode = TMVA::gTools().xmlengine().DocGetRootElement(doc); //node "MethodSetup"
  TString fullMethodName("");
  TMVA::gTools().ReadAttr(rootnode, "Method", fullMethodName);

  std::cout<<std::endl;
  std::cout<<"  ===================================================="<<std::endl;
  std::cout<<" |                Cut Opt Configuration               |"<<std::endl;
  std::cout<<"  ----------------------------------------------------"<<std::endl;
  std::cout<<" | "<<std::setiosflags(std::ios::left)<<std::setw(10)<<"Method"<<" | "<<std::setw(37)<<fullMethodName<<" |"<<std::endl;

  ofresult<<std::endl;
  ofresult<<"  ===================================================="<<std::endl;
  ofresult<<" |                Cut Opt Configuration               |"<<std::endl;
  ofresult<<"  ----------------------------------------------------"<<std::endl;
  ofresult<<" | "<<std::setiosflags(std::ios::left)<<std::setw(10)<<"Method"<<" | "<<std::setw(37)<<fullMethodName<<" |"<<std::endl;

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

  void* variables = TMVA::gTools().GetChild(rootnode,"Variables");
  UInt_t nVar=0;
  std::vector<TString> varnames;
  TMVA::gTools().ReadAttr(variables, "NVar", nVar);
  void* var = TMVA::gTools().GetChild(variables,"Variable");
  for(unsigned int k=0;k<nVar;k++)
    {
      TString varname("");
      TMVA::gTools().ReadAttr(var, "Expression", varname);
      TString tem = Form("Variable%i",k);
      std::cout<<"  ----------------------------------------------------"<<std::endl;
      std::cout<<" | "<<std::setiosflags(std::ios::left)<<std::setw(10)<<tem<<" | "<<std::setw(37)<<varname<<" |"<<std::endl;
      ofresult<<"  ----------------------------------------------------"<<std::endl;
      ofresult<<" | "<<std::setiosflags(std::ios::left)<<std::setw(10)<<tem<<" | "<<std::setw(37)<<varname<<" |"<<std::endl;
      var = TMVA::gTools().GetNextChild(var);
      varnames.push_back(varname);
    }
  std::cout<<"  ===================================================="<<std::endl;
  ofresult<<"  ===================================================="<<std::endl;
  std::cout<<std::endl;

  /*
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
  */

  TFile* inputS = new TFile(inputSname.Data());
  TFile* inputB = new TFile(inputBname.Data());

  TTree* background = (TTree*)inputB->Get("ntKp");
  background->AddFriend("ntHlt");
  background->AddFriend("ntHi");
  background->AddFriend("ntSkim");
  background->AddFriend(Form("%s_pt_%.0f_%.0f",mvatype.Data(),ptmin,ptmax));

  TTree* signal = (TTree*)inputS->Get("ntKp");
  signal->AddFriend("ntHlt");
  signal->AddFriend("ntHi");
  signal->AddFriend("ntSkim");
  signal->AddFriend(Form("%s_pt_%.0f_%.0f",mvatype.Data(),ptmin,ptmax));

  TTree* generated = (TTree*)inputS->Get("ntGen");
  generated->AddFriend("ntHlt");
  generated->AddFriend("ntHi");

  for(int i=0;i<=nMVA;i++) gmvaBins[i] = minMVA+i*widMVA;
  TH1F* hcountEffS = new TH1F("hcountEffS",Form(";%s;Signal counts",mvatype.Data()),nMVA,minMVA,maxMVA);
  TH1F* hcountEffB = new TH1F("hcountEffB",Form(";%s;Background counts",mvatype.Data()),nMVA,minMVA,maxMVA);

  signal->Project("hcountEffS",Form("%s_pt_%.0f_%.0f",mvatype.Data(),ptmin,ptmax),Form("%s*(%s)",weight.Data(),cuts.Data()));
  background->Project("hcountEffB",Form("%s_pt_%.0f_%.0f",mvatype.Data(),ptmin,ptmax),Form("%s",cutb.Data()));

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

  TH2F* hemptydis = new TH2F("hemptydis",Form(";%s;Probability",mvatype.Data()),50,-1.2,1.2,10,0.,hdisEffB->GetMaximum()*1.4);
  sethempty(hemptydis);
  /*
  hemptydis->GetXaxis()->CenterTitle();
  hemptydis->GetYaxis()->CenterTitle();
  hemptydis->GetXaxis()->SetTitle(Form("%s",mvatype.Data()));
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

  TLatex* texPar = new TLatex(0.18,0.93, Form("%s 5.02 TeV B^{+}",collisionsyst.Data()));
  texPar->SetNDC();
  texPar->SetTextAlign(12);
  texPar->SetTextSize(0.04);
  texPar->SetTextFont(42);
  */
  TLatex* texPt = new TLatex(0.21,0.81, Form("%.1f < B^{+} p_{T} < %.1f GeV",ptmin,ptmax));
  texPt->SetNDC();
  texPt->SetTextAlign(32);
  texPt->SetTextSize(0.04);
  texPt->SetTextFont(42);
  TLatex* texY = new TLatex(0.21,0.75, "|y| < 2.4");
  texY->SetNDC();
  texY->SetTextAlign(32);
  texY->SetTextSize(0.04);
  texY->SetTextFont(42);

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
  drawCMS(collisionsyst);
  texPt->Draw();
  texY->Draw();
  cdisEffSB->SaveAs(Form("%s_%s.pdf",outputmvadis.Data(),outputname.Data()));

  //Float_t allS=0,allB=0;
  Float_t effS[nMVA+1],effB[nMVA+1],effSig[nMVA+1];
  for(int i=0;i<(nMVA+1);i++)
    {
      effS[i] = 0;
      effB[i] = 0;
      effSig[i] = 0;
    }
  for(int i=0;i<nMVA;i++)
    {
      effS[i] = hcountEffS->Integral(i+1,nMVA)/hcountEffS->Integral();
      effB[i] = hcountEffB->Integral(i+1,nMVA)/hcountEffB->Integral();
    }
  /*
  for(int i=0;i<(nMVA+1);i++)
    {
      allS+=hcountEffS->GetBinContent(i+1);
      allB+=hcountEffB->GetBinContent(i+1);
      if(i==0) continue;
      for(int j=0;j<i;j++)
        {
          if(j<nMVA)
            {
              effS[j]+=hcountEffS->GetBinContent(i+1);
              effB[j]+=hcountEffB->GetBinContent(i+1);
            }
        }
    }
  for(int i=0;i<nMVA;i++)
    {
      effS[i]/=allS;
      effB[i]/=allB;
    }
  */

  TH2F* hemptyeff = new TH2F("hemptyeff",Form(";%s;Efficiency",mvatype.Data()),50,-1.2,1.2,10,0.,1.4);
  sethempty(hemptyeff);
  /*
  hemptyeff->GetXaxis()->CenterTitle();
  hemptyeff->GetYaxis()->CenterTitle();
  hemptyeff->GetXaxis()->SetTitle(Form("%s",mvatype.Data()));
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
  */
  TGraph* geffS = new TGraph(nMVA+1,gmvaBins,effS);
  geffS->SetMarkerSize(1.1);
  geffS->SetMarkerColor(kRed);
  geffS->SetLineWidth(2);
  geffS->SetLineColor(kRed);
  TGraph* geffB = new TGraph(nMVA+1,gmvaBins,effB);
  geffB->SetMarkerSize(1.1);
  geffB->SetMarkerColor(kBlue+1);
  geffB->SetLineWidth(2);
  geffB->SetLineColor(kBlue+1);
  TCanvas* ceffSB = new TCanvas("ceffSB","",600,600);
  hemptyeff->Draw();
  geffS->Draw("samepl");
  geffB->Draw("samepl");
  TLegend* legeff = new TLegend(0.60,0.75,0.95,0.86);
  legeff->SetBorderSize(0);
  legeff->SetFillStyle(0);
  legeff->AddEntry(geffB,"Background","pl");
  legeff->AddEntry(geffS,"Signal","pl");
  legeff->Draw("same");
  drawCMS(collisionsyst);
  texPt->Draw();
  texY->Draw();
  ceffSB->SaveAs(Form("%s_%s.pdf",outputefficiency.Data(),outputname.Data()));

  Float_t wSignal=0;
  Float_t wBackground=0;
  Float_t* weights = new Float_t[2];
  //
  calRatio(signal, background, generated, cuts, cutb, cutg, weight, ptmin, ptmax, "fonlls/fo_pp_Bplus_5p03TeV_y2p4.dat", lumi, raa, outputname, weights);
  //

  wSignal = weights[1];
  wBackground = weights[0];

  std::cout<<std::endl;
  std::cout<<"Looking for max significance ..."<<std::endl;

  //Double_t maxsig = wSignal*effS[0]/sqrt(wSignal*effS[0]+wBackground*effB[0]);
  Double_t maxsig = 0;
  int maxindex = 0;
  for(int i=0;i<nMVA;i++)
    {
      effSig[i] = wSignal*effS[i]/sqrt(wSignal*effS[i]+wBackground*effB[i]);
      if(effSig[i]>maxsig)
	{
	  maxsig=effSig[i];
	  maxindex=i;
	}
    }

  // Print out opt results
  std::cout<<std::endl;
  std::cout<<"  =========================================================="<<std::endl;
  std::cout<<" |                         Opt Result                       |"<<std::endl;
  std::cout<<"  ----------------------------------------------------------"<<std::endl;
  std::cout<<" | "<<std::setiosflags(std::ios::left)<<std::setw(15)<<"pT"<<" | "<<std::setw(38)<<Form("%.1f - %.1f GeV/c",ptmin,ptmax)<<" |"<<std::endl;
  std::cout<<"  ----------------------------------------------------------"<<std::endl;
  std::cout<<" | "<<std::setiosflags(std::ios::left)<<std::setw(15)<<"RAA"<<" | "<<std::setw(38)<<raa<<" |"<<std::endl;
  std::cout<<"  ----------------------------------------------------------"<<std::endl;
  std::cout<<" | "<<std::setiosflags(std::ios::left)<<std::setw(15)<<"Sig reg"<<" | "<<std::setw(9)<<Form("+- %.2f",dmassBsignal)<<" | "<<std::setw(13)<<"Sideband bkg"<<" | "<<std::setw(10)<<Form("%.0f",wBackground*(dmassBsidbandH-dmassBsidbandL)/dmassBsignal)<<" |"<<std::endl;
  std::cout<<"  ----------------------------------------------------------"<<std::endl;
  std::cout<<" | "<<std::setiosflags(std::ios::left)<<std::setw(15)<<"weight S"<<" | "<<std::setw(9)<<Form("%.0f",wSignal)<<" | "<<std::setw(13)<<"weight B"<<" | "<<std::setw(10)<<Form("%.0f",wBackground)<<" |"<<std::endl;
  std::cout<<"  ----------------------------------------------------------"<<std::endl;
  std::cout<<" | "<<std::setiosflags(std::ios::left)<<std::setw(15)<<"S"<<" | "<<std::setw(9)<<Form("%.0f",wSignal*effS[maxindex])<<" | "<<std::setw(13)<<"B"<<" | "<<std::setw(10)<<Form("%.0f",wBackground*effB[maxindex])<<" |"<<std::endl;
  std::cout<<"  ----------------------------------------------------------"<<std::endl;
  std::cout<<" | "<<std::setiosflags(std::ios::left)<<std::setw(15)<<"effS"<<" | "<<std::setw(9)<<effS[maxindex]<<" | "<<std::setw(13)<<"sig"<<" | "<<std::setw(10)<<Form("%.2f",effSig[maxindex])<<" |"<<std::endl;
  std::cout<<"  ----------------------------------------------------------"<<std::endl;
  std::cout<<" | "<<std::setiosflags(std::ios::left)<<std::setw(35)<<mvatype<<" | "<<std::setw(18)<<gmvaBins[maxindex]<<" |"<<std::endl;
  std::cout<<"  =========================================================="<<std::endl;
  std::cout<<std::endl;

  ofresult<<std::endl;
  ofresult<<"  =========================================================="<<std::endl;
  ofresult<<" |                         Opt Result                       |"<<std::endl;
  ofresult<<"  ----------------------------------------------------------"<<std::endl;
  ofresult<<" | "<<std::setiosflags(std::ios::left)<<std::setw(15)<<"pT"<<" | "<<std::setw(38)<<Form("%.1f - %.1f GeV/c",ptmin,ptmax)<<" |"<<std::endl;
  ofresult<<"  ----------------------------------------------------------"<<std::endl;
  ofresult<<" | "<<std::setiosflags(std::ios::left)<<std::setw(15)<<"RAA"<<" | "<<std::setw(38)<<raa<<" |"<<std::endl;
  ofresult<<"  ----------------------------------------------------------"<<std::endl;
  ofresult<<" | "<<std::setiosflags(std::ios::left)<<std::setw(15)<<"Sig reg"<<" | "<<std::setw(9)<<Form("+- %.2f",dmassBsignal)<<" | "<<std::setw(13)<<"Sideband bkg"<<" | "<<std::setw(10)<<Form("%.0f",wBackground*(dmassBsidbandH-dmassBsidbandL)/dmassBsignal)<<" |"<<std::endl;
  ofresult<<"  ----------------------------------------------------------"<<std::endl;
  ofresult<<" | "<<std::setiosflags(std::ios::left)<<std::setw(15)<<"weight S"<<" | "<<std::setw(9)<<Form("%.0f",wSignal)<<" | "<<std::setw(13)<<"weight B"<<" | "<<std::setw(10)<<Form("%.0f",wBackground)<<" |"<<std::endl;
  ofresult<<"  ----------------------------------------------------------"<<std::endl;
  ofresult<<" | "<<std::setiosflags(std::ios::left)<<std::setw(15)<<"S"<<" | "<<std::setw(9)<<Form("%.0f",wSignal*effS[maxindex])<<" | "<<std::setw(13)<<"B"<<" | "<<std::setw(10)<<Form("%.0f",wBackground*effB[maxindex])<<" |"<<std::endl;
  ofresult<<"  ----------------------------------------------------------"<<std::endl;
  ofresult<<" | "<<std::setiosflags(std::ios::left)<<std::setw(15)<<"effS"<<" | "<<std::setw(9)<<effS[maxindex]<<" | "<<std::setw(13)<<"sig"<<" | "<<std::setw(10)<<Form("%.2f",effSig[maxindex])<<" |"<<std::endl;
  ofresult<<"  ----------------------------------------------------------"<<std::endl;
  ofresult<<" | "<<std::setiosflags(std::ios::left)<<std::setw(35)<<mvatype<<" | "<<std::setw(18)<<gmvaBins[maxindex]<<" |"<<std::endl;
  ofresult<<"  =========================================================="<<std::endl;
  ofresult<<std::endl;

  ofresult.close();

  TH2F* hempty = new TH2F("hempty",Form(";%s;S/sqrt(S+B)",mvatype.Data()),50,-1.2,1.2,10,0.,maxsig*1.4);
  sethempty(hempty);
  /*
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
  */

  TGraph* gsig = new TGraph(nMVA+1,gmvaBins,effSig);
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
      readxml(argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], argv[7], argv[8], argv[9], argv[10], atof(argv[11]), atof(argv[12]), atof(argv[13]), atof(argv[14]));
      return 0;
    }
  if(argc==14)
    {
      readxml(argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], argv[7], argv[8], argv[9], argv[10], atof(argv[11]), atof(argv[12]), atof(argv[13]));
      return 0;
    }
  else
    {
      std::cout<<"  Error: invalid argument number - readxml()"<<std::endl;
      return 1;
    }
}

