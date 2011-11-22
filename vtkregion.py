#!/usr/bin/env python

#
# This example introduces the concepts of user interaction with VTK.
# First, a different interaction style (than the default) is defined.
# Second, the interaction is started. 
#
#

import vtk
import sys
import random
# 
# Next we create an instance of vtkConeSource and set some of its
# properties. The instance of vtkConeSource "cone" is part of a visualization
# pipeline (it is a source process object); it produces data (output type is
# vtkPolyData) which other filters may process.
#
reader = vtk.vtkUnstructuredGridReader()
reader.SetFileName("/Users/hamid_r_ghadyani/fooka.vtk")
#

reader.Update()
uGrid = reader.GetOutput()
cellData = uGrid.GetPointData()
if cellData.SetActiveScalars("region") == -1:
	print " No region data in the file."
	sys.exit()

subdomainArray = cellData.GetScalars()
smax=-1
smin=100
for x in subdomainArray.GetRange():
	if x>smax:
		smax = x
	if x<smin:
		smin = x
		
print smin, smax

nsubdomains = smax - smin
colorstep = 0.9/nsubdomains
color = 0.05

ren2 = vtk.vtkRenderer()
ren2.SetBackground(1,1,1)

random.seed(1.2)

smin = int(smin)
smax = int(smax)
c=0
subs=[]
geoFilters=[]
subMappers=[]
subActors=[]

for i in range(smin,smax+1):
	subs.append(vtk.vtkThreshold())
	subs[c].SetInputArrayToProcess(subdomainArray)
	subs[c].SetInput(uGrid)
	subs[c].ThresholdBetween(i-0.5,i+0.5)

	geoFilters.append(vtk.vtkGeometryFilter())
	geoFilters[c].SetInput(subs[c].GetOutput())
	
	subMappers.append(vtk.vtkPolyDataMapper())
	subMappers[c].SetInput(geoFilters[c].GetOutput())
	subMappers[c].ScalarVisibilityOff()
	
	subActors.append(vtk.vtkActor())
	subActors[c].SetMapper(subMappers[c])
	subActors[c].GetProperty().SetColor(color, random.random(), random.random())
	
	ren2.AddActor(subActors[c])
	
	color = color + colorstep
	c = c + 1

renWin2 = vtk.vtkRenderWindow()
renWin2.AddRenderer(ren2)
renWin2.SetSize(300,300)

iren2 = vtk.vtkRenderWindowInteractor()
iren2.SetRenderWindow(renWin2)

iren2.Initialize()
iren2.Start()


iren.Initialize()
iren.Start()
