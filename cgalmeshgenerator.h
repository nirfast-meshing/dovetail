#ifndef CGALMESHGENERATOR_H
#define CGALMESHGENERATOR_H

#include "metaimageio.h"

#include "cgal_header.h"

#include <vtkDataObjectToTable.h>
#include <vtkElevationFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkQtTableView.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkVectorText.h>

#include "vtkPolyDataReader.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkExtractEdges.h"
#include "vtkCamera.h"
#include "vtkActor.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkProperty.h"
#include "vtkPoints.h"
#include "vtkSmartPointer.h"
#include "vtkGeometryFilter.h"
#include "vtkTubeFilter.h"
#include "vtkAxesActor.h"
#include "vtkOrientationMarkerWidget.h"
#include "vtkTransform.h"

#include "CGAL/IO/Complex_3_in_triangulation_3_to_vtk.h"

#define VTK_CREATE(type, name) \
  vtkSmartPointer<type> name = vtkSmartPointer<type>::New()

class CGALMeshGenerator
{
public:
    CGALMeshGenerator();
    CGALMeshGenerator(std::string &);
    CGALMeshGenerator(MetaImageIO&);

    int Execute();
    int SetInputDomain(MetaImageIO &);

    size_t NoOfVertices() { return _no_vertices; }
    size_t NoOfCells() { return _no_cells; }

    C3t3 c3t3;

    double facet_angle, facet_size, facet_distance,
           cell_radius_edge;
    double general_cell_size;
    double special_size;         // Cell size to be used in subdomains of image with 'special_subdomain_label'
    int special_subdomain_label; // If this is set to zero, no subdomain resizing will be performed

    bool metaimage_error;
    std::string outFilename;
private:
    MetaHeader metaimage_header;
    void initialize();
    bool _inrWritten;
    std::string _inrFilename;
    size_t _no_vertices;
    size_t _no_cells;
};
#endif // CGALMESHGENERATOR_H
