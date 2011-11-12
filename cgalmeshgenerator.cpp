#include "cgalmeshgenerator.h"

CGALMeshGenerator::CGALMeshGenerator() : metaimage_error(false)
{
    initialize();
}

CGALMeshGenerator::CGALMeshGenerator(MetaImageIO &metaimage) : metaimage_error(false), _inrWritten(false)
{
    initialize();
    if (!metaimage.INRWritten())
    {
        if (metaimage.WriteINR()!=0)
            metaimage_error = true;
    }
    _inrFilename = metaimage.inrFilename;
    _inrWritten = true;

    // Setup the outFilename
    QFileInfo fi1(_inrFilename.c_str());
    QFileInfo fi2;
    QString foo1 = fi1.completeBaseName();
    QString foo2(".mesh");
    foo1 += foo2;

    fi2.setFile(fi1.dir(),foo1);
    outFilename = fi2.absoluteFilePath().toStdString();
    std::cout << "\noutFilename is cgal is:" << outFilename << '\n';
}

int CGALMeshGenerator::Execute()
{
    if (_inrFilename.empty() || outFilename.empty())
        return 1;
    // Domain
    CGAL::Image_3 image;
    Mesh_domain domain(image);
    image.read(_inrFilename.c_str());

    // Sizing field: set global size to general_cell_size
    // and special size (label special_subdomain_label) to special_size
    Sizing_field size(general_cell_size);
    if (special_subdomain_label) {
        std::cout << " Refining domain with label ID: " << special_subdomain_label << std::endl;
        size.set_size(special_size, 3,
                      domain.index_from_subdomain_index(special_subdomain_label));
    }

    // Mesh criteria
    Mesh_criteria criteria(facet_angle, facet_size, facet_distance,
                           cell_radius_edge, cell_size=size);

    // Meshing
    C3t3 c3t3 = CGAL::make_mesh_3<C3t3>(domain, criteria);

    // Output
    std::ofstream medit_file(outFilename.c_str());
    c3t3.output_to_medit(medit_file);
    return 0;
}

void CGALMeshGenerator::initialize()
{
   facet_angle = 25;
   facet_size = 3.0;
   facet_distance = 0.5;
   cell_radius_edge = 3;
   general_cell_size = 3.0;
   special_size = 2;
   special_subdomain_label = 0;
   outFilename = "";
   _inrFilename = "";
}
