from PyQt4 import QtCore, QtGui
import vtk
import sys

class Ui_MainWindow(object):
	def setupUi(self, MainWindow):
		MainWindow.setObjectName("MainWindow")
		MainWindow.resize(400, 400)
		self.centralWidget = QtGui.QWidget(MainWindow)
		self.gridlayout = QtGui.QGridLayout(self.centralWidget)
		self.vtkWidget = vtk.QVTKWidget(self.centralWidget)
		self.gridlayout.addWidget(self.vtkWidget, 0, 0, 1, 1)
		MainWindow.setCentralWidget(self.centralWidget)

class SimpleView(QtGui.QMainWindow):
    
	def __init__(self, parent = None):
	
		QtGui.QMainWindow.__init__(self, parent)
		self.ui = Ui_MainWindow() 
		self.ui.setupUi(self)
		
		widget = self.ui.vtkWidget
		
		ren = vtk.vtkRenderer()
		renwin = widget.GetRenderWindow()
		renwin.AddRenderer(ren)
		
		cone = vtk.vtkSphereSource()
		sphere = vtk.vtkSphereSource()
		gfilter = vtk.vtkGeometryFilter()
		gfilter.AddInputConnection(cone.GetOutputPort())
		gfilter.AddInputConnection(1,sphere.GetOutputPort())
		
		mapper = vtk.vtkPolyDataMapper()
		mapper.SetInputConnection(gfilter.GetOutputPort())
		actor = vtk.vtkActor()
		actor.SetMapper(mapper)
		
		self._w = vtk.vtkOrientationMarkerWidget()
		iren = ren.GetRenderWindow().GetInteractor()
		self._w.SetDefaultRenderer(ren)
		self._w.SetInteractor(iren);
		
		axes = vtk.vtkAxesActor()
		self._w.SetOrientationMarker(axes)
		
		self._w.SetOutlineColor(1,0,0)
		self._w.SetEnabled(1)
		self._w.InteractiveOn()
		
		ren.AddViewProp(actor)
		ren.ResetCamera()
				
if __name__ == "__main__":

    app = QtGui.QApplication(sys.argv)
    window = SimpleView()
    window.show()
    sys.exit(app.exec_())