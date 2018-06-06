
# Example: displaying a ROOT histogram from Python
from ROOT import gRandom,TCanvas,TH1F,TFile # this is the "include" of python.. what does "from ROOT" mean?

c1 = TCanvas('c1','Example',200,10,700,500) # the same method as in c++ (wow!)
hpx = TH1F('hpx','px',100,-4,4)
for i in xrange(25000): px = gRandom.Gaus(); hpx.Fill(px)
hpx.Draw()
c1.Update()

# some lines to save output
file = TFile("HelloWorld_output.root","recreate")
print "Hello world. While I said this I also saved a .pdf with a gaussian distribution."
hpx.Write("hpx")
file.Close()
print "Ciao!"
