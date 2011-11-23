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

#include "CGAL/IO/Complex_3_in_triangulation_3_to_vtk.h"

#define VTK_CREATE(type, name) \
  vtkSmartPointer<type> name = vtkSmartPointer<type>::New()

// Vector_3<Kernel> v ( double x, double y, double z);
// Aff_transformation_3<Kernel> t ( const Translation, Vector_3<Kernel> v);
// Point_3<Kernel> t.operator() ( Point_3<Kernel> p) const
//class addoffset : public std::unary_function<Tr::Vertex,void>
//{
//    K::Vector_3 offset;
//public:
//    explicit addoffset(double x, double y, double z) : offset(K::Vector_3(x,y,z)) {}

//    void operator()(Tr::Vertex& v)
//    {
//        v.point()=Weighted_point(v.point().point()+offset, v.point().weight());
//    }
//};

//        for (Vertices_iterator itv=c3t3.triangulation().finite_vertices_begin(),
//             itv_end=c3t3.triangulation().finite_vertices_end();
//             itv!=itv_end;++itv)
//        {
//            itv->point()=
//                    Weighted_point(itv->point().point()+offset, itv->point().weight());
//        }
//    }


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
