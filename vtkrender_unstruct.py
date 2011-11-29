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
# reader.SetFileName("/Users/hamid_r_ghadyani/fooka.vtk")
reader.SetFileName("/Volumes/Home/foo/1.vtk")

geomFilter = vtk.vtkGeometryFilter()
geomFilter.SetInput(reader.GetOutput())

# mapper = vtk.vtkDataSetMapper()
# mapper.SetInput(reader.GetOutput())

elementsMapper = vtk.vtkPolyDataMapper()
elementsMapper.SetInput(geomFilter.GetOutput())

elementsActor = vtk.vtkActor()
elementsActor.SetMapper(elementsMapper)

elementsProp = elementsActor.GetProperty()
elementsProp.SetColor(0.91,0.87,0.67)
elementsProp.SetDiffuse(0)
elementsProp.SetAmbient(1)
elementsProp.SetInterpolationToFlat()


edgesFilter1 = vtk.vtkExtractEdges()
edgesFilter2 = vtk.vtkExtractEdges()
edgesFilter1.SetInput(reader.GetOutput())
edgesFilter2.SetInput(geomFilter.GetOutput())

tubeFilter = vtk.vtkTubeFilter()
tubeFilter.SetInput(edgesFilter2.GetOutput())
tubeFilter.SetRadius(0.1)

edgesMapper = vtk.vtkPolyDataMapper()
edgesMapper.SetInput(tubeFilter.GetOutput())
edgesMapper.ScalarVisibilityOff()

edgesActor = vtk.vtkActor()
edgesActor.SetMapper(edgesMapper)

edgesProp = edgesActor.GetProperty()
edgesProp.SetColor(0,0,0)
edgesProp.SetDiffuse(0)
edgesProp.SetAmbient(1)
edgesProp.SetLineWidth(2)

ren = vtk.vtkRenderer()
ren.SetBackground(1,1,1)
ren.AddActor(elementsActor)
ren.AddActor(edgesActor)

renWin = vtk.vtkRenderWindow()
renWin.AddRenderer(ren)
renWin.SetSize(300,300)

iren = vtk.vtkRenderWindowInteractor()
iren.SetRenderWindow(renWin)

style = vtk.vtkInteractorStyleTrackballCamera()
iren.SetInteractorStyle(style)

#

reader.Update()
uGrid = reader.GetOutput()
cellData = uGrid.GetCellData()
if cellData.SetActiveScalars("mua") == -1:
    print " No region data in the file."
    sys.exit()

subdomainArray = cellData.GetScalars()
for x in subdomainArray.GetRange():
    if x>smax:
        smax = x
    if x<smin:
        xmin = x
nsubdomains = smax - smin
colorstep = 0.9/nsubdomains
color = 0.05

ren2 = vtk.vtkRenderer()
ren2.SetBackground(1,1,1)

random.seed(1.2)

smin = int(smin)
smax = int(smax)
c=0
for i in range(smin,smax+1):
    subs[c] = vtk.vtkThreshold()
    subs[c].SetArrayName("mua")
    subs[c].SetAttributeModeToUseCellData()
    subs[c].SetInput(uGrid)
    subs[c].ThresholdBetween(i-0.5,i+0.5)

    geoFilters[c] = vtk.GeometryFilter()
    geoFilters[c].SetInput(subs[c].GetOutput())
    
    subMappers[c] = vtk.PolyDataMapper()
    subMappers[c].SetInput(geoFilters.GetOutput())
    subMappers[c].ScalarVisibilityOff()
    
    subActors[c] = vtk.vtkActors()
    subActors[c].SetMapper(subMappers[c])
    subActors[c].SetColor(color, random.random(), random.random())
    
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
