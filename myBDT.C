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


//Directory where the signal/data imput files are stored
string SAMPLES_DIR = "/eos/user/a/ancsmith/DispLepNtuples/MicroNtuples/";

//parameters for signal input: 200_0_1 --> 200(GeV) selectron with 1(ns) lifetime
string GRID_POINT = "200_0_10";

//final state reconstructed by ATLAS: ee, eg, gg, 1g, 1e (e --> electron, g --> photon)
string FINAL_STATE = "1e";

//name of input data file
string DATA_FILE = "data22_partial";

int myBDT(){

    int NsigTrain, NbkgTrain, NsigTest, NbkgTest;

    //cuts applied to data sample. NOTE need to require t < 0 !!! this blinds the data 
    // because our signal is expected to be enriched with events with t > 0
    //For example:
    //el1_t < 0 --> electron (el) with highest pt (1)  has time of flight (t) < 0 
    //you can use PostSel_1e->Print() in interactive root to see available variables
    TCut BkgCuts = "el1_t < 0 && abs(el1_t) < 12.5i && (el1_eta < 1.37 || el1_eta > 1.52)"; 

    //cuts applied to signal sample. Generally should be the same as those applied to data,
    // except the blinding cut which is reversed (t > 0)
    TCut SigCuts = "el1_t > 0 && abs(el1_t) < 12.5 && (el1_eta < 1.37 || el1_eta > 1.52)";

    //size of training set and testing set (default of 0 will split datasets in half)
    NsigTrain = 0;
    NsigTest = 0;
    NbkgTrain = 10000;
    NbkgTest = 10000;


    const TString & splitOpt  = Form("nTrain_Signal=%i:nTrain_Background=%i:nTest_Signal=%i:nTest_Background=%i", NsigTrain, NbkgTrain, NsigTest, NbkgTest);

    //Here we declare the "factory" and "dataloader" objects used to process the input
    TFile *out = new TFile("output.root","RECREATE");

    TMVA::Factory* factory = new TMVA::Factory( "TMVAClassification", out, "" );
    TMVA::DataLoader *dataloader=new TMVA::DataLoader("dataset");


    //open input signal and background files
    TFile *sigSrc = new TFile(Form("%s/signal/%s.root",SAMPLES_DIR.c_str(),GRID_POINT.c_str()),"READ");
    TFile *dataSrc = new TFile(Form("%s/data/%s.root",SAMPLES_DIR.c_str(),DATA_FILE.c_str()),"READ");

    //get trees from input signal and background files
    TTree* sigTree  = (TTree*)sigSrc->Get(  Form("PostSel_%s",FINAL_STATE.c_str())  );
    TTree* bkgTree  = (TTree*)dataSrc->Get(  Form("PostSel_%s",FINAL_STATE.c_str()) );


    //scale signal and background distributions by constant multiplier
    Double_t sigWeight  = 1.0;
    Double_t bkgWeight  = 1.0;


    dataloader->AddSignalTree( sigTree,  sigWeight  );
    dataloader->AddBackgroundTree( bkgTree, bkgWeight );

    //the Monte Carlo sample events are each given a weight when they were generated, we should apply this weight.
    dataloader->SetSignalWeightExpression("wt" );

    //ADD INPUT VARIABLES ()

    //syntax: dataloader->AddVariable( $variable_name_from_tree, $fancy_title, $variable type (usually F for float))

    dataloader->AddVariable( "el1_eta",  "|#eta|","", 'F' );

    dataloader->AddVariable( "el1_maxEcell_E",  "E_{maxCell}","[mm]", 'F' );
    dataloader->AddVariable( "el1_dpt",  "#Delta p_{T}/p_{T}","", 'F' );
    dataloader->AddVariable( "el1_chi2",  "#Chi^{2}","", 'F' );
    dataloader->AddVariable( "el1_pt",  "p_{T}","", 'F' );
    dataloader->AddVariable( "el1_nPix","N_{Pix Hits}","",'I');

    //dataloader->AddVariable( "abs(electron_time)",  "t_e","[ns]", 'F' );
    //dataloader->AddVariable( "abs(electron_d0)",  "|d_0|","[mm]", 'F' );

    dataloader->PrepareTrainingAndTestTree(SigCuts,BkgCuts,splitOpt);

    //with BookMethod, you can add an additional argument with options for training the tree
    // see table 25 and 26 here: https://root.cern.ch/download/doc/tmva/TMVAUsersGuide.pdf (p130-133)

    //EXAMPLE:  factory->BookMethod( dataloader, TMVA::Types::kBDT, "BDT",
    //          "!H:!V:NTrees=850:MinNodeSize=2.5%:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:UseBaggedBoost:BaggedSampleFraction=0.5:SeparationType=GiniIndex:nCuts=20" );

    factory->BookMethod(  dataloader, TMVA::Types::kBDT, "BDT" ); 

    factory->TrainAllMethods();
    factory->TestAllMethods();
    factory->EvaluateAllMethods();

    delete factory;
    return 0;
}
