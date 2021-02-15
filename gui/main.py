import sys
from PyQt5 import uic
from PyQt5.QtCore import *
from PyQt5.QtWidgets import * 
from PyQt5.QtGui import *
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.backends.backend_qt5agg import NavigationToolbar2QT as NavigationToolbar
import matplotlib.pyplot as plt
from subprocess import Popen, PIPE, STDOUT, check_output
import os
from numpy import savetxt
import res3


class MyThread(QThread):

	trigger = pyqtSignal(float)
	completed = pyqtSignal()

	def __init__(self, parent=None):
		super(MyThread, self).__init__(parent)

	def setup(self, process=None, beginframe=0, endframe=0):
		self.process = process
		self.beginframe = beginframe
		self.endframe = endframe

	def run(self):
		#print ("Thread launched.")
		while True:
			line = self.process.stdout.readline()
			if b"Opening frame" in line:
				line  = str(line)
				#print (line, end='')
				beginstr = line.index("[")+1
				endstr = line.index("]")
				currentframe = int(line[beginstr:endstr])
				stat = float(currentframe-self.beginframe+1)/(self.endframe-self.beginframe)
				self.trigger.emit(stat)			
			elif b"Cannot open snapshot" in line:
				print ("ERROR: invalid frame specified")
				self.process.stdout.close()
				break

			if not line: break	

		self.completed.emit()			




class ImportDialog(QDialog):
	
	def __init__(self):
		super(ImportDialog, self).__init__()
		uic.loadUi('importgro.ui', self)
		self.local_folder = None
		self.inputgrofile = None
		working_path = os.path.dirname(os.path.realpath(__file__))
		self.script_path = working_path + os.sep+"algorithm.py"
		print(" * HyperBeta's algorithm will be run from:", self.script_path)


	def openmd(self):
		file_, _ = QFileDialog.getOpenFileName(None, 'Select a GROMACS file:', self.local_folder, "GROMACS files (*.gro)")
		if file_!="":
			print (" * Opening GRO animation file", file_)
			self.inputgro.setText(file_)


	def breakdown(self, fi): 

		self.local_folder = os.path.dirname(fi)
		self.inputgrofile = os.path.basename(fi)
		os.chdir(self.local_folder)

		print (" * Processing", fi)	
		#dirname = "./exploded_"+os.path.basename(fi)			
		#print " exploding in directory", dirname
		dirname = "."

		with open(fi) as infi:			
			A = infi.readlines()		
		B = zip(A,range(len(A)))
		C = list(filter(lambda x: "Generated" in x[0], B))
		if len(C)<2:
			import shutil
			shutil.copyfile(fi, dirname+"/snapshot_0")
			return
		
		I = [i[1] for i in C]
		for n in range(len(I)-1):
			nextfile = dirname+"/snapshot_"+str(n)
			if os.path.isfile(nextfile) and self.skipexisting.isChecked():
				print (nextfile, "already exists: skipping")
			else:
				with open(dirname+"/snapshot_"+str(n), "w") as fo: #versione normale
					for i in range(I[n], I[n+1]):
						fo.write(A[i])


	def processmd(self):

		# break down files
		self.breakdown(str(self.inputgro.text()))

		# save groupsize
		savetxt("grouplength", [int(self.groups.text())], fmt="%d")
		
		# launch script
		ang_threshold = float(self.angthreshold.value())
		dist_threshold = float(self.distancethreshold.value())
		first_snapshot = int(self.firstsnapshot.value())
		last_snapshot = int(self.lastsnapshot.value())
		dirname = "./exploded_"+os.path.basename(str(self.inputgro.text()))			
		
		QApplication.setOverrideCursor(Qt.WaitCursor)
		for i in range(first_snapshot, last_snapshot):
			command = ["python", self.script_path, ".", "snapshot_"+str(i), "-a", str(ang_threshold), "-d", str(dist_threshold) ]
			print (" ".join(command))
			ret = check_output(command)
			print (ret)
			#break
		QApplication.restoreOverrideCursor()
		
		


class MyWindow(QMainWindow):

	
	def __init__(self):
		super(MyWindow, self).__init__()
		uic.loadUi('main.ui', self)
		
		self.show()
		
		self.progressBar = QProgressBar()
		self.progressBar.setMinimum(0)
		self.progressBar.setMaximum(100)
		self.statusBar().addPermanentWidget(self.progressBar)


		try:
			with open(".last") as fi:
				self.path.setText( fi.readline() )
			print (" * Opening last saved file:", self.path.text())
		except:
			print ("WARNING: last file not found")
		try:
			with open(".begin") as fi:
				self.begin.setValue( int(fi.readline()) )
			print (" * Opening first frame:", self.begin.value())
		except:
			print ("WARNING: last file not found")
		try:
			with open(".end") as fi:
				self.end.setValue( int(fi.readline()) )
			print (" * Opening last frame:", self.end.value())
		except:
			print ("WARNING: last file not found")
		try:
			with open(".exec") as fi:
				self.path_to_hvt.setText(fi.readline())
			print (" * Setting simulator to %s" % self.path_to_hvt.text())
		except:
			print ("WARNING: simlulator executable file not found")

		self.check_sim()

	def check_sim(self):
		if os.path.exists(str(self.path_to_hvt.text())):
			self.statusBar().showMessage("Simulator online")
		else:
			self.statusBar().showMessage("Simulator not found")

	def browse_folders(self):				
		local = os.path.abspath(str(self.path.text()))
		print (local)
		#if local=="": local= os.path.abspath(".")
		dir_ = QFileDialog.getExistingDirectory(None, 'Select a folder:', local, QFileDialog.ShowDirsOnly)
		if dir_!="":
			self.path.setText(dir_)
			with open(".last", "w") as fo:
				fo.write(dir_)

	def browse_executable(self):				
		local = os.path.abspath(str(self.path.text()))
		print (local)
		executable_, _ = QFileDialog.getOpenFileName(self, 'Open executable file', '.', "Image files (*.exe)")
		if executable_!="":
			self.path_to_hvt.setText(executable_)
			with open(".exec", "w") as fo:
				fo.write(executable_)
		self.check_sim()

	def _save_config(self):
		with open(".begin", "w") as fo:
			fo.write(str(self.begin.value()))
		with open(".end", "w") as fo:
			fo.write(str(self.end.value()))
		with open(".exec", "w") as fo:
			fo.write(str(self.path_to_hvt.text()))

	def launch(self):
		self._save_config()

		beginframe = int(self.begin.value())
		endframe = int(self.end.value())

		command = [
			str(self.path_to_hvt.text()), 
			str(self.path.text()), 
			str(self.filename.text()), 
			str(beginframe), 
			str(endframe)
		]

		working_dir =  os.path.dirname(os.path.realpath(command[0]))
		p = Popen(command, stdout = PIPE, stderr = STDOUT, shell = True, cwd="../HVT")

		self.progressBar.setVisible(True)

		thread = MyThread(self)
		thread.trigger.connect(self.update) 		
		thread.completed.connect(self.done) 		
		thread.setup(process=p, beginframe=beginframe, endframe=endframe)
		thread.start() 
		#print("?")


	def update(self,value):
		self.progressBar.setValue(value*100)		
		app.processEvents()

	def done(self):
		self.progressBar.setVisible(False)		
		print (" * 3D visualization terminated")



	def preprocess_gro(self):		
		importgro.local_folder = os.path.abspath(str(self.path.text()))
		importgro.show()


if __name__ == '__main__':

	app = QApplication(sys.argv)
	importgro = ImportDialog()
	window = MyWindow()
	
	window.show()
	sys.exit(app.exec_())