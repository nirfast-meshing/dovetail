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

#endif // CGALMESHGENERATOR_H
