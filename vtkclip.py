#!/usr/bin/env python

#
# This example introduces the concepts of user interaction with VTK.
# First, a different interaction style (than the default) is defined.
# Second, the interaction is started. 
#
#

import vtk

# 
# Next we create an instance of vtkConeSource and set some of its
# properties. The instance of vtkConeSource "cone" is part of a visualization
# pipeline (it is a source process object); it produces data (output type is
# vtkPolyData) which other filters may process.
#
reader = vtk.vtkUnstructuredGridReader()
# reader = vtk.vtkPolyDataReader();
reader.SetFileName("/Users/hamid/double.vtk")
reader.Update();
# 
# In this example we terminate the pipeline with a mapper process object.
# (Intermediate filters such as vtkShrinkPolyData could be inserted in
# between the source and the mapper.)  We create an instance of
# vtkPolyDataMapper to map the polygonal data into graphics primitives. We
# connect the output of the cone souece to the input of this mapper.
#
moldMapper = vtk.vtkDataSetMapper()
# moldMapper = vtk.vtkPolyDataMapper()
moldMapper.SetInputConnection(reader.GetOutputPort())

edges = vtk.vtkExtractEdges()
edges.SetInput(reader.GetOutput())
edge_mapper = vtk.vtkPolyDataMapper()
edge_mapper.SetInput(edges.GetOutput())

# 
# Create an actor to represent the cone. The actor orchestrates rendering of
# the mapper's graphics primitives. An actor also refers to properties via a
# vtkProperty instance, and includes an internal transformation matrix. We
# set this actor's mapper to be coneMapper which we created above.
#
coneActor = vtk.vtkActor()
edgeActor = vtk.vtkActor()
edgeActor.SetMapper(edge_mapper)
edgeActor.GetProperty().SetColor(0.2,0.3,0.4)
coneActor.SetMapper(moldMapper)

#
# Create the Renderer and assign actors to it. A renderer is like a
# viewport. It is part or all of a window on the screen and it is responsible
# for drawing the actors it has.  We also set the background color here.
#
ren1 = vtk.vtkRenderer()
# ren1.AddActor(coneActor)
# ren1.AddActor(edgeActor)
ren1.SetBackground(0.1, 0.2, 0.1)

vtk.vtkPolyDataMapper().SetResolveCoincidentTopologyToPolygonOffset()

#
# Cutting Stuff
#
plane = vtk.vtkPlane()
plane.SetOrigin(reader.GetOutput().GetCenter())
plane.SetNormal(0,0,1);

planeCut = vtk.vtkCutter()
planeCut.SetInputConnection(reader.GetOutputPort())
planeCut.SetCutFunction(plane)

cutMapper = vtk.vtkPolyDataMapper()
cutMapper.SetInputConnection(planeCut.GetOutputPort())
cutMapper.SetScalarRange(reader.GetOutput().GetScalarRange())

cutActor = vtk.vtkActor()
cutActor.SetMapper(cutMapper)

ren1.AddActor(cutActor)

#
# Clipping Stuff
#
# clip = vtk.vtkClipPolyData()
clip = vtk.vtkClipDataSet()
clip.SetInputConnection(reader.GetOutputPort())
clip.SetClipFunction(plane)
clip.InsideOutOn()

# clipMapper = vtk.vtkPolyDataMapper()
clipMapper = vtk.vtkDataSetMapper()
clipMapper.SetInputConnection(clip.GetOutputPort())
clipMapper.SetScalarRange(reader.GetOutput().GetScalarRange())

clipActor = vtk.vtkActor()
clipActor.SetMapper(clipMapper)

prop = clipActor.GetProperty()
prop.SetColor(0.9,0.8,0.6)
prop.SetDiffuse(0)
# prop.SetAmbient(1)
prop.SetInterpolation(1)

ren1.AddActor(clipActor)

edges2 = vtk.vtkExtractEdges()
edges2.SetInputConnection(clip.GetOutputPort())
edge_mapper2 = vtk.vtkPolyDataMapper()
edge_mapper2.SetInput(edges2.GetOutput())

edges2Actor = vtk.vtkActor()
edges2Actor.SetMapper(edge_mapper2)
edges2Actor.GetProperty().SetColor(0.8,0.8,0.8)
ren1.AddActor(edges2Actor)
#
# Finally we create the render window which will show up on the screen
# We put our renderer into the render window using AddRenderer. We also
# set the size to be 300 pixels by 300.
#
renWin = vtk.vtkRenderWindow()
renWin.AddRenderer(ren1)
renWin.SetSize(300, 300)

# 
# The vtkRenderWindowInteractor class watches for events (e.g., keypress,
# mouse) in the vtkRenderWindow. These events are translated into
# event invocations that VTK understands (see VTK/Common/vtkCommand.h
# for all events that VTK processes). Then observers of these VTK
# events can process them as appropriate.
iren = vtk.vtkRenderWindowInteractor()
iren.SetRenderWindow(renWin)

#
# By default the vtkRenderWindowInteractor instantiates an instance
# of vtkInteractorStyle. vtkInteractorStyle translates a set of events
# it observes into operations on the camera, actors, and/or properties
# in the vtkRenderWindow associated with the vtkRenderWinodwInteractor. 
# Here we specify a particular interactor style.
style = vtk.vtkInteractorStyleTrackballCamera()
iren.SetInteractorStyle(style)

_w = vtk.vtkOrientationMarkerWidget()

# _w.SetDefaultRenderer(ren1)
_w.SetInteractor(iren);

axes = vtk.vtkAxesActor()
_w.SetOrientationMarker(axes)

_w.SetOutlineColor(1,0,0)
_w.SetEnabled(1)
_w.InteractiveOn()
_w.SetViewport(0,0,0.4,0.4)


#
# Initialize and start the event loop. Once the render window appears, mouse
# in the window to move the camera. The Start() method executes an event
# loop which listens to user mouse and keyboard events. Note that keypress-e
# exits the event loop. (Look in vtkInteractorStyle.h for a summary of events, or
# the appropriate Doxygen documentation.)
#
ren1.ResetCamera()
iren.Initialize()
iren.Start()
