#include <cstdlib>
#include <iostream>
#include <map>
#include <string>

#include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TObjString.h"
#include "TSystem.h"
#include "TROOT.h"

#include "TMVA/Factory.h"
#include "TMVA/DataLoader.h"
#include "TMVA/Tools.h"
#include "TMVA/TMVAGui.h"

int myBDT(){

    int NsigTrain, NbkgTrain, NsigTest, NbkgTest;
    TCut SigCuts = "reco_region == 1 && electron_time > 0";
    TCut BkgCuts = "reco_region == 1 && electron_time < 0";

    NsigTrain = 0;
    NsigTest = 0;
    NbkgTrain = 0;
    NbkgTest = 0;


    const TString & splitOpt  = Form("nTrain_Signal=%i:nTrain_Background=%i:nTest_Signal=%i:nTest_Background=%i", NsigTrain, NbkgTrain, NsigTest, NbkgTest);

    //Here we declare the "factory" used to process the input"
    TFile *out = new TFile("output.root","RECREATE");

    TMVA::Factory* factory = new TMVA::Factory( "TMVAClassification", out, "" );
    TMVA::DataLoader *dataloader=new TMVA::DataLoader("dataset");


    //open input signal and background files
    TFile *sigSrc = new TFile("/eos/user/a/ancsmith/DispLepNtuples/signal/v2p0/100_0_1.root","READ");
    TFile *dataSrc = new TFile("/eos/user/a/ancsmith/DispLepNtuples/data/data22_test.root","READ");

    //get trees from input signal and background files
    TTree* sigTree  = (TTree*)sigSrc->Get(  "trees_SR_oneEM_"  );
    TTree* bkgTree  = (TTree*)dataSrc->Get(  "trees_SR_oneEM_"  );


    //scale signal and background
    Double_t sigWeight  = 1.0;
    Double_t bkgWeight = 1.0;


    dataloader->AddSignalTree( sigTree,  sigWeight  );
    dataloader->AddBackgroundTree( bkgTree, bkgWeight );
    dataloader->SetSignalWeightExpression( "normweight*mcEventWeight*1000" );

    //ADD INPUT VARIABLES
    dataloader->AddVariable( "electron_eta",  "|#eta|","", 'F' );
    //dataloader->AddVariable( "abs(electron_time)",  "t_e","[ns]", 'F' );
    //dataloader->AddVariable( "abs(electron_d0)",  "|d_0|","[mm]", 'F' );
    dataloader->AddVariable( "electron_maxEcell_E",  "E_{maxCell}","[mm]", 'F' );
    dataloader->AddVariable( "electron_dpt",  "#Delta p_{T}/p_{T}","", 'F' );
    dataloader->AddVariable( "electron_chi2",  "#Chi^{2}","", 'F' );
    dataloader->AddVariable( "electron_pt",  "p_{T}","", 'F' );
    dataloader->AddVariable("electron_nPIX","N_{Pix Hits}","",'I');

    dataloader->PrepareTrainingAndTestTree(SigCuts,BkgCuts,splitOpt);
    factory->BookMethod(  dataloader, TMVA::Types::kBDT, "BDT" ); 
    factory->TrainAllMethods();
    factory->TestAllMethods();
    factory->EvaluateAllMethods();

    delete factory;
    return 0;
}