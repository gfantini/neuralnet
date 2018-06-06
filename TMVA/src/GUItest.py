# Glue-ing Qt and ROOT through Python
import sys, ROOT
from qt import *

theApp = QApplication( sys.argv)
box = QVBox();
box.resize(QSize(40,10).expandedTo(box.minimumSizeHint()))
class myButton(QPushButton):
    def __init__( self,label,master):
        QPushButton.__init__(self,label,master)
        self.setFont( QFont('Times',18,QFont.Bold))
    def browse(self):
        self.b = ROOT.TBrowser()

bb = myButton('browser',box)
QObject.connect( bb,SIGNAL('clicked()'),bb.browse)
theApp.setMainWidget(box)
box.show()
theApp.exec_loop()
