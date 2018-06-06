from ROOT import TFile, TTree, gRandom
from array import array #need it to load data into TTree

# file creation
outFileName = "../input/myTMVA.root"
outFile     = TFile(outFileName,"recreate");
print "[MakeInputTMVA.py] Hello, I just created ", outFileName
 
TreeS = TTree("TreeS","Tree of signal events TMVA")
TreeB = TTree("TreeB","Tree of background events TMVA")
x1 = array("f",[0.]);
x2 = array("f",[0.]);
TreeS.Branch("x1",x1,"x1/F")
TreeS.Branch("x2",x2,"x2/F")
TreeB.Branch("x1",x1,"x1/F")
TreeB.Branch("x2",x2,"x2/F")

for i in range(100000):
    #signal
    x1[0] = gRandom.Gaus(1.,1.)
    x2[0] = gRandom.Gaus(10.,1.)
    TreeS.Fill()
    #background
    x1[0] = gRandom.Gaus(2.,1.)
    x2[0] = gRandom.Gaus(9.,0.8)
    TreeB.Fill()
#-------------------------------
outFile.Write()
outFile.Close()
print "[MakeInputTMVA.py] Goodbye!"


#### HOW TO DECLARE ARRAYS IN PYTHON
#maxn = 10
#n = array('i',[0]) # array int[1]
#d = array('f',maxn*[0.]) # array float[maxn]


#### HOW TO USE TTree INTO PYTHON
## open the file
#myfile = TFile('tree1.root')
## retrieve the ntuple of interest
#mychain = myfile.Get('t1')
#entries = mychain.GetEntriesFast()
#for jentry in xrange(entries):
## get the next tree in the chain and verify ientry = mychain.LoadTree(jentry)
#if ientry < 0:
#break
## copy next entry into memory and verify
#nb = mychain.GetEntry(jentry) if nb<=0:
#continue
## use the values directly from the tree
#nEvent = int(mychain.ev)
