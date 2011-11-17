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

    metaimage_header = metaimage.myheader;

    // Setup the outFilename
    if (outFilename.empty())
    {
        QFileInfo fi1(_inrFilename.c_str());
        QFileInfo fi2;
        QString foo1 = fi1.completeBaseName();
        QString foo2(".mesh");
        foo1 += foo2;

        fi2.setFile(fi1.dir(),foo1);
        outFilename = fi2.absoluteFilePath().toStdString();
        std::cout << "\noutFilename of cgal is:" << outFilename << '\n';
        std::cout.flush();
    }
}

int CGALMeshGenerator::SetInputDomain(MetaImageIO &metaimage)
{
    if (!metaimage.INRWritten())
    {
        if (metaimage.WriteINR()!=0)
        {
            metaimage_error = true;
            return 1;
        }
    }
    _inrFilename = metaimage.inrFilename;
    _inrWritten = true;
    metaimage_header = metaimage.myheader;

    // Setup the outFilename
    QFileInfo fi1(_inrFilename.c_str());
    QFileInfo fi2;
    QString foo1 = fi1.completeBaseName() + ".mesh";

    fi2.setFile(fi1.dir(),foo1);
    outFilename = fi2.absoluteFilePath().toStdString();
    std::cout << "\noutFilename of cgal is:" << outFilename << '\n';
    std::cout.flush();
    return 0;
}

int CGALMeshGenerator::Execute()
{
    if (_inrFilename.empty() || outFilename.empty())
        return 1;
    // Domain
    CGAL::Image_3 image;
    if (image.read(_inrFilename.c_str()) != true)
        return 1;

    Mesh_domain domain(image);



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


//    Tr tr = c3t3.triangulation();
    K::Vector_3 _offset(
                metaimage_header.offset[0],
                metaimage_header.offset[1],
                metaimage_header.offset[2]);

    // Apply the offset
    for (Vertices_iterator itv=c3t3.triangulation().finite_vertices_begin(),
         itv_end=c3t3.triangulation().finite_vertices_end();
         itv!=itv_end;++itv)
    {
        itv->point()=
                Weighted_point(itv->point().point()+_offset, itv->point().weight());
    }

    // Output
    std::ofstream medit_file(outFilename.c_str());
    c3t3.output_to_medit(medit_file);

    _no_vertices = c3t3.triangulation().number_of_vertices();
    _no_cells = c3t3.number_of_cells_in_complex();

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
   _no_vertices = 0;
   _no_cells = 0;
}
