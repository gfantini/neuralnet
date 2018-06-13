// qrpp TMVA_MLP.C -o ../bin/TMVA_MLP -I/cuore/soft/root_v5.34.34/include/ -L/cuore/soft/root_v5.34.34/lib/  -lTMVA  

#include <cstdlib>
#include <iostream>
#include <map>
#include <string>
#include <fstream>

#include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TObjString.h"
#include "TSystem.h"
#include "TROOT.h"

#include "TMVA/Factory.h"
#include "TMVA/Tools.h"

using namespace std;

// expect input root to have trees TreeS / TreeB with V[SamplingFrequency*WindowLength] FLOAT
const string folder = "/nfs/cuore1/scratch/gfantini/neuralnet/TMVA/";
// reccommended Neural Networks: MLP / MLPBFGS (+optional training method) / MLPBNN (+bayesian)
void MLP_Classification(string logfilename = "log/TMVA_MLP.log",string rootfilename_out = "output/TMVA_MLP.root",string rootfilename_in = "input/TMVA_MLP.root"){
  // set up logfile and output rootfile
  ofstream logfile;
  logfile.open( (folder+logfilename).c_str() );
  logfile << "--> OutputFolder: " << folder << endl;
  logfile << "--> Created logfile " << folder+logfilename << endl;
  TFile *pOutputRoot = new TFile( (folder+rootfilename_out).c_str() ,"RECREATE");
  if(!pOutputRoot->IsOpen()){
    logfile << "FATAL! Could not open output rootfile. Abort." << endl;
    logfile << "File: " << folder + rootfilename_out << endl;
    exit(0);
  }
  logfile << "--> Opened output root " << folder + rootfilename_out << endl;  
  // define input root
  TFile *pInputRoot = new TFile( (folder+rootfilename_in).c_str(), "READ");
  if(!pOutputRoot->IsOpen()){
    logfile << "FATAL! Could not open input rootfile. Abort." << endl;
    logfile << "File: " << folder + rootfilename_in << endl;
    exit(0);
  }
  logfile << "--> Opened input root " << folder + rootfilename_in << endl;
  // fetch TTree
  TTree *signal = 0;
  signal = (TTree*)pInputRoot->Get("TreeS");
  TTree *background = 0;
  background = (TTree*)pInputRoot->Get("TreeB");
  if(signal == 0 || background == 0){
    logfile << "FATAL! Could not get signal or background TTree." << endl;
    exit(0);
  }
  
  // 1. define factory
  TMVA::Factory *factory = new TMVA::Factory( "TMVAClassification", pOutputRoot,
					      "!V:!Silent:Color:DrawProgressBar:Transformations=I;D;P;G,D:AnalysisType=Classification" );
  // 2. define variables you want to use            
  const int SamplingFrequency = 1000; // Hz               
  const int WindowLength = 10; // s              
  for(int i=0; i<SamplingFrequency*WindowLength; i++)
    factory->AddVariable(Form("V%d := V[%d]",i,i),"mV","F");
  // 3. set options for signal and background tree and add them
  factory->AddSignalTree    ( signal,     1.);
  factory->AddBackgroundTree( background, 1.);
  TCut mycuts = "";
  TCut mycutb = "";
  factory->PrepareTrainingAndTestTree( mycuts, mycutb,
				       "nTrain_Signal=0:nTrain_Background=0:SplitMode=Random:NormMode=NumEvents:!V" );
  // 4. select the algorithm: MLP (MultiLayer Perceptron), reccommended ANN
  /*
   * H                          [general] prints help messages, stats, hints on algo...
   * V                          [general] verbose flag
   * NeuronType                 [MLP]     neuron activation function ?? which are available?
   * VarTransform               [general] variable transformation before processing 
   * NCycles                    [...]
   * HiddenLayers               [MLP]     number of neurons in the hidden layers separated by comma, e.g.:
   *                                      1 hidden layer  N+5
   *                                      2 hidden layers N,N+1 
   * TestRate                   [MLP]     number of epochs (times you scan the whole training set) after which perform overtraining test
   * UseRegulator               [MLP]     algo to avoid overtraining --> Bayesian extension of MLP
   *************************************************************************************************************************************
   * Default options:           H:!V:NeuronType=tanh:VarTransform=N:NCycles=600:HiddenLayers=N+5:TestRate=5:!UseRegulator
   */
  factory->BookMethod( TMVA::Types::kMLP, "MLP", 
		       "H:!V:NeuronType=tanh:VarTransform=N:NCycles=600:HiddenLayers=N+5:TestRate=5:!UseRegulator" );

  // 5. rock and roll
  factory->TrainAllMethods();
  factory->TestAllMethods();
  factory->EvaluateAllMethods();
  
  // 6. save the output, do some cleanup
  pOutputRoot->Close();
  logfile << "--> Wrote logfile." << endl;
  logfile << "--> TMVA classification is done." << endl;
  delete factory;
}


int main(int argc,char** argv)
{
  if(argc != 4){
    cout << "call ./executable <logfilename> <outputROOT> <inputROOT>" << endl;
    cout << "All paths will be relative to " << folder << endl;
    return 1;
  }else{
    string logfile = argv[1];
    string output = argv[2];
    string input = argv[3];
    MLP_Classification(logfile,output,input);
  }
  return 0;
}
