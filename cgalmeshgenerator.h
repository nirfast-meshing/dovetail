#ifndef CGALMESHGENERATOR_H
#define CGALMESHGENERATOR_H

#include "metaimageio.h"

#include "cgal_header.h"

class CGALMeshGenerator
{
public:
    CGALMeshGenerator();
    CGALMeshGenerator(std::string &);
    CGALMeshGenerator(MetaImageIO&);
    int Execute();

    double facet_angle, facet_size, facet_distance,
           cell_radius_edge;
    double general_cell_size;
    double special_size;         // Cell size to be used in subdomains of image with 'special_subdomain_label'
    int special_subdomain_label; // If this is set to zero, no subdomain resizing will be performed

    bool metaimage_error;
    std::string outFilename;
private:
    void initialize();
    bool _inrWritten;
    std::string _inrFilename;
};


// Vector_3<Kernel> v ( double x, double y, double z);
// Aff_transformation_3<Kernel> t ( const Translation, Vector_3<Kernel> v);
// Point_3<Kernel> t.operator() ( Point_3<Kernel> p) const
class addoffset : public std::unary_function<Finte_vertices_iterator,Finte_vertices_iterator>
{
    Tr::Point offset;

public:
    explicit addoffset(Finite_vertices_iterator& _other) : offset(_other) {}
    Finte_vertices_iterator operator()(Finte_vertices_iterator& inp) const
    {
        inp->point()->x() += offset.point().x();
        return inp;
    }
}

#endif // CGALMESHGENERATOR_H
