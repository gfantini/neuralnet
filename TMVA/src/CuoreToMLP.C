// compile with qrpp CuoreToMLP -o ../bin/CuoreToMLP

// include files below are needed only if the macro is being compiled with ACLiC 
#if !defined(__CINT__) || defined(__MAKECINT__)
#include "TCanvas.h"
#include "TGraph.h"
#include "TFile.h"
#include "TAxis.h"
// files below can be included only if diana-root.C has been executed,
// which sets the CUORE software include paths.
#include "QChain.hh"
#include "QHeader.hh"
#include "QPulse.hh"
#include "QPulseInfo.hh"
#include "QBaseType.hh"
#include "QRunDataHandle.hh"
#include "QGlobalDataManager.hh"
#include "QBaselineData.hh"
#include "QCountPulsesData.hh"
#include "QBool.hh"
#endif

// Avoid putting Cuore:: in front of objects defined in the Cuore namespace
using namespace Cuore;
using namespace std;

const int Nprintout = 10000;

void CuoreToMLP(string inputList = "Blinded_301530_C.list",string outputFolder = "/nfs/cuore1/scratch/gfantini/neuralnet/TMVA/input/",string folder = "/nfs/cuore1/scratch/gfantini/mydiana/output/ds3021/")
{
  // meant to be run interactively 
  string filename = folder + inputList;
  QChain* chain = new QChain();
  if(!chain->Add(filename.c_str())){
    cout<<"Error while reading file: "<<filename<<endl;
    return;
  }

  // Set branches of raw event QObject%s and of analysis QObject%s if present.
  // Remember to add a dot after the branch name.
  // Quantities below are the raw event
  QHeader* header = 0;
  QPulse* pulse = 0;
  QPulseInfo* pulseInfo = 0;
  chain->SetBranchAddress("DAQ@Header.",&header);
  chain->SetBranchAddress("DAQ@Pulse.",&pulse);
  chain->SetBranchAddress("DAQ@PulseInfo.",&pulseInfo);
  // baseline things 
  QDouble* maxBaseline = 0;
  QBaselineData* baselineData = 0;
  QCountPulsesData* countPulses = 0;
  if(chain->FindBranch("PulseBasicParameters@MaxBaseline."))  {
    chain->SetBranchAddress("PulseBasicParameters@MaxBaseline.",&maxBaseline);
    chain->SetBranchAddress("BaselineModule@BaselineData.",&baselineData);
    chain->SetBranchAddress("BCountPulses@CountPulsesData.",&countPulses);
  }
  // sampleinfo
  QBool* pSampleInfoFilter= 0; // This is SingleTrigger                                      
  chain->SetBranchAddress("SampleInfoFilter@Passed.",&pSampleInfoFilter);
  // bad intervals
  QBool* pRejectBadIntervals= 0;
  chain->SetBranchAddress("RejectBadIntervals_AntiCoincidence_Tower@Passed.",&pRejectBadIntervals);

  // open root for output                     
  TFile* pOutputFile = new TFile( (outputFolder + "InputTMVA.root").c_str(),"RECREATE");
  cout << "Created output file" << (outputFolder + "InputTMVA.root") << endl;
  // define output TTree
  TTree* TreeS = new TTree("TreeS","Signal Tree");
  TTree* TreeB = new TTree("TreeB","Background Tree");
  Float_t Vs[1000*10];
  Float_t Vb[1000*10];
  TreeS->Branch("V",&Vs[0],"V[10000]/F");
  TreeB->Branch("V",&Vb[0],"V[10000]/F");

  // Get number of entries in the chain
  Long64_t nentries = chain->GetEntries();
  if(nentries == 0) {
    cout<<"Empty files!"<<endl;
    return;
  }
  cout<<"Number of entries: "<<nentries<<endl;
  
  // loop on TChain entries
  Bool_t IsSignal;
  Bool_t SingleTrigger;
  Int_t NumberOfPulses;
  Bool_t RejectBadIntervals;
  Double_t BaselineSlope;
  double* my_pulse;
  for(Long64_t entry = 0; entry < nentries; entry++) {
    if(entry%Nprintout == 1) cout << 100.*(double)entry/(double)nentries << " % completed." << endl;
    // get QChain entry 
    chain->GetEntry(entry);
    // get interesting variables
    IsSignal           = pulseInfo->GetIsSignal();
    SingleTrigger      = *pSampleInfoFilter;
    NumberOfPulses     = countPulses->GetNumberOfPulses();
    RejectBadIntervals = *pRejectBadIntervals;
    BaselineSlope      = baselineData->GetBaselineSlope();
    // select signal
    if(IsSignal && RejectBadIntervals && abs(BaselineSlope) < 0.01){// base cuts
      if(SingleTrigger && NumberOfPulses == 1){// signal
	my_pulse = (pulse->GetSamples()).GetArray();
	if((pulse->GetSamples()).Size() != 10000){// check..
	  cerr << "FATAL! Samples: " << (pulse->GetSamples()).Size() <<" expected 10000" << endl;
	  exit(0);
	}
	for(int i=0;i<10000; i++)Vs[i] = my_pulse[i];
	TreeS->Fill();
      }
      if(NumberOfPulses > 1){// background
	my_pulse = (pulse->GetSamples()).GetArray();
	if((pulse->GetSamples()).Size() != 10000){// check..      
	  cerr << "FATAL! Samples: "<< (pulse->GetSamples()).Size() <<" expected 10000" << endl;
	  exit(0);
	}
	for(int i=0;i<10000; i++)Vb[i] = my_pulse[i];
	TreeB->Fill();
      }
    }
  }
  // finalize..
  TreeS->Write("TreeS");
  TreeB->Write("TreeB");
  pOutputFile->Close();
  delete pOutputFile;
  // . . . .
  cout << "Goodbye!"<< endl;
}


int main(int argc, char* argv[])
{

  CuoreToMLP();
  return 0;


}
