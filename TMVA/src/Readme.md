### Neural Networks for CUORE

## TMVA_MLP
This is an implementation of a Multi Layer Perceptron Neural Net that requires an input root file with 2 TTree: TreeS and TreeB
for signal and background pulses. Each tree needs to have a V[1000*10]/F leaf with the samples of the pulses stored.

## CuoreToMLP
This reads .list from the CUORE data and produces a .root file with the TreeS and TreeB required by TMVA_MLP
A set of cuts is implemented inside to select signal / background events.
It takes some time to run -> there is a .pbs script in the pbs folder to run it batch