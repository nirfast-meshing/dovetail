#include "image2mesh.h"
#include "ui_image2mesh.h"

#include <QtGui>

int Run_CGALMeshGenerator(MetaImageIO& _mi);

Image2Mesh::Image2Mesh(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Image2Mesh)
{
    ui->setupUi(this);
    ui->pushButton_GenerateMesh->setEnabled(false);
    ui->pushButton_ViewMesh->setEnabled(false);
    // Regional refinement
    ui->lineEdit_SpecialSize1->setText("0");
    ui->lineEdit_SpecialID1->setText("0");

    // Meshing settings
    ui->lineEdit_TetSize->setText("3.0");
    ui->lineEdit_TetQual->setText("3.0");
    ui->lineEdit_FacetAngle->setText("25.0");
    ui->lineEdit_FacetDistance->setText("3.0");
    ui->lineEdit_FacetSize->setText("3.0");

    ui->textEdit_StatusInfo->setReadOnly(true);
    this->_populatedVTKPolyData = false;
    this->imageDataLoaded = false;
//    connect(this, SIGNAL(UpdateImageProperties(QString)), ui->lineEdit_SDfilename, SLOT(setText(QString)));
}

Image2Mesh::~Image2Mesh()
{
    delete ui;
}

void Image2Mesh::on_pushButton_BrowseImage_clicked()
{
    QString imageFile = QFileDialog::getOpenFileName(this, tr("Select Image file(s)"),
                                                     "",tr("Images (*.bmp *.tiff *.png *.mha)"),
                                                     NULL, QFileDialog::ReadOnly);
    _loadImage(imageFile);
}

void Image2Mesh::UpdateMeshingCriteria()
{
    ui->textEdit_StatusInfo->clear();
    if (mi.ImageLabels.size() > 10)
    {
        QString foo = "There are more than 10 regions in the image!";
        foo += "\nMesh generation process can be lengthy and unsuccessful.";
        ui->textEdit_StatusInfo->setText(foo);
    }
    ui->lineEdit_TetQual->setText(ui->lineEdit_TetQual->text());
    ui->lineEdit_FacetAngle->setText(ui->lineEdit_FacetAngle->text());
    ui->lineEdit_FacetDistance->setText(ui->lineEdit_FacetDistance->text());

    std::vector<double>::iterator mind =
            std::min_element(mi.myheader.elementsize.begin(), mi.myheader.elementsize.end());
    ui->lineEdit_TetSize->setText(QString::number(*mind,'f',4));
    ui->lineEdit_FacetSize->setText(QString::number(*mind,'f',4));

    ui->textEdit_RegionInfo->clear();
    ui->textEdit_RegionInfo->setReadOnly(true);
    QString foo = QString("");
//    std::cout << "labels length: " << mi.ImageLabels.size() << std::endl;
    for (unsigned int i=0; i<mi.ImageLabels.size(); ++i)
    {
        foo += QString::number(mi.ImageLabels[i],'f',0) + ", ";
    }
    foo.remove(foo.length()-2,2);
    foo.push_back('.');
    ui->textEdit_RegionInfo->setText(foo);
    ui->label_AvailRegionIDs->setText(
                QString("Avail. region IDs in image:")+
                QString(" (total of ")+
                QString::number(mi.ImageLabels.size())+
                QString(")"));

}

void Image2Mesh::UpdateImageProperties()
{
    char s[512];
    sprintf(s,"%d",mi.myheader.dimsize[0]);
    ui->lineEdit_Rows->setText(QString(s));
    ui->lineEdit_Rows->setEnabled(false);

    sprintf(s,"%d",mi.myheader.dimsize[1]);
    ui->lineEdit_Cols->setText(QString(s));
    ui->lineEdit_Cols->setEnabled(false);

    sprintf(s,"%d",mi.myheader.dimsize[2]);
    ui->lineEdit_Slices->setText(QString(s));
    ui->lineEdit_Slices->setEnabled(false);

    sprintf(s,"%.6f",mi.myheader.elementsize[0]);
    ui->lineEdit_X->setText(s);

    sprintf(s,"%.6f",mi.myheader.elementsize[1]);
    ui->lineEdit_Y->setText(s);

    sprintf(s,"%.6f",mi.myheader.elementsize[2]);
    ui->lineEdit_Z->setText(s);

    sprintf(s,"%.4f",mi.myheader.offset[0]);
    ui->lineEdit_OffsetX->setText(s);

    sprintf(s,"%.4f",mi.myheader.offset[1]);
    ui->lineEdit_OffsetY->setText(s);

    sprintf(s,"%.4f",mi.myheader.offset[2]);
    ui->lineEdit_OffsetZ->setText(s);
}

void Image2Mesh::GetImageProperties()
{
    mi.myheader.elementsize[0] =  ui->lineEdit_X->text().toDouble();
    mi.myheader.elementsize[1] =  ui->lineEdit_Y->text().toDouble();
    mi.myheader.elementsize[2] =  ui->lineEdit_Z->text().toDouble();
    mi.myheader.offset[0] = ui->lineEdit_OffsetX->text().toDouble();
    mi.myheader.offset[1] = ui->lineEdit_OffsetY->text().toDouble();
    mi.myheader.offset[2] = ui->lineEdit_OffsetZ->text().toDouble();

}

void Image2Mesh::GetMeshCriteria()
{
    mesher.cell_radius_edge = ui->lineEdit_TetQual->text().toDouble();
    mesher.general_cell_size = ui->lineEdit_TetSize->text().toDouble();
    mesher.special_size = ui->lineEdit_SpecialSize1->text().toDouble();
    mesher.special_subdomain_label = ui->lineEdit_SpecialID1->text().toInt();

    mesher.facet_angle = ui->lineEdit_FacetAngle->text().toDouble();
    mesher.facet_distance = ui->lineEdit_FacetDistance->text().toDouble();
    mesher.facet_size = ui->lineEdit_FacetSize->text().toDouble();
}

void Image2Mesh::on_pushButton_GenerateMesh_clicked()
{
    // TODO
    // Get the output file name and construct corresponding
    // inr filename and set inrFilename before calling
    // CGALMeshGenerator
    ui->pushButton_GenerateMesh->setEnabled(false);


    GetImageProperties();
    GetMeshCriteria();

    if (Run_CGALMeshGenerator() != 0)
        ui->textEdit_StatusInfo->setText("\nMesh generation failed!");
    else
    {
        ui->textEdit_StatusInfo->clear();
        QString foo = "\nMesh generation completed.";
        foo += "\n No of nodes: " + QString::number(mesher.NoOfVertices());
        foo += "\n No of elements: " + QString::number(mesher.NoOfCells());
        ui->textEdit_StatusInfo->setText(foo);
        int st = PopulateVTKPolyData();
        if (st != 0)
        {
            ui->textEdit_StatusInfo->insertPlainText(" Can not display this mesh!");
        }
        else
        {
            ui->textEdit_StatusInfo->insertPlainText("Click 'View Mesh' now!");
            ui->pushButton_ViewMesh->setEnabled(true);
        }
    }

    ui->pushButton_GenerateMesh->setEnabled(true);
}

int Image2Mesh::Run_CGALMeshGenerator()
{
    ui->textEdit_StatusInfo->clear();
    ui->textEdit_StatusInfo->setText("\nGenerating Mesh, please wait");
    mesher.SetInputDomain(mi);
    if (mesher.metaimage_error)
        return 1;
    else
    {
        if (mesher.Execute() != 0)
            return 1; // Unsuccessful
        else
            return 0;
    }

}

std::string Image2Mesh::makeFileName(std::string _fn, std::string _ext)
{
    QFileInfo fi1(_fn.c_str());
    QString foo = fi1.completeBaseName() + QString::fromStdString(_ext);
    fi1.setFile(fi1.dir(), foo);
    return fi1.absoluteFilePath().toStdString();
}

void Image2Mesh::on_lineEdit_infilename_returnPressed()
{
    _loadImage(ui->lineEdit_infilename->text());
    std::cout << "lineEdit_infilename: " << ui->lineEdit_infilename->text().toStdString();
}

void Image2Mesh::_loadImage(QString imageFile)
{
    if (! imageFile.isEmpty() ) {
            if (QFileInfo(imageFile).suffix() == "mha")
            {
                ui->lineEdit_infilename->setText(imageFile);
                mi.myheader = MetaHeader();
                mi._filename = imageFile.toStdString();

                ui->pushButton_GenerateMesh->setEnabled(false);

                if (mi.ReadHeader() != 0)
                {
                    std::cerr << " Could not read " << imageFile.toStdString() << std::endl;
                    ui->textEdit_StatusInfo->setText(QString(" Could not read file: ") +
                                                     QString::fromStdString(mi._filename));
                    QMessageBox::information(0,"error",QString(" Could not read file: ") +
                                             QString::fromStdString(mi._filename));
                    ui->textEdit_RegionInfo->clear();
                    return;
                }
                if (mi.ReadVolData() != 0)
                {
                    std::cerr << " Could not read " << mi.DataFileName() << std::endl;
                    ui->textEdit_StatusInfo->setText(QString(" Could not read file: ") +
                                                     QString::fromStdString(mi._filename));
                    QMessageBox::information(0,"error",QString(" Could not read file: ") +
                                             QString::fromStdString(mi._filename));
                    ui->textEdit_RegionInfo->clear();
                    return;
                }

                mesher.outFilename = makeFileName(mi._filename,std::string(".mesh"));
                ui->lineEdit_outputfilename->setText(QString::fromStdString(mesher.outFilename));

                UpdateImageProperties();
                UpdateMeshingCriteria();

                ui->pushButton_GenerateMesh->setEnabled(true);
            }
            this->lastImageFile = imageFile;
        }
        else
            ui->lineEdit_infilename->clear();
}

void Image2Mesh::on_pushButton_ViewMesh_clicked()
{
    QString imageFile("/Users/hamid_r_ghadyani/2.vtk");

    ui->tabWidget->setCurrentIndex(1);
    VTK_CREATE(vtkPolyDataReader, reader);
    reader->SetFileName(imageFile.toStdString().c_str());
    reader->Update();
    VTK_CREATE(vtkPolyDataMapper, meshMapper);
    meshMapper->ImmediateModeRenderingOn();
    meshMapper->SetInputConnection(reader->GetOutputPort());

    VTK_CREATE(vtkExtractEdges, edges);
    edges->SetInput(reader->GetOutput());
    VTK_CREATE(vtkPolyDataMapper, edge_mapper);
    edge_mapper->ImmediateModeRenderingOn();
    edge_mapper->SetInput(edges->GetOutput());




    VTK_CREATE(vtkActor, meshActor);
    VTK_CREATE(vtkActor, edgeActor);
    meshActor->SetMapper( meshMapper );
    edgeActor->SetMapper( edge_mapper );
    edgeActor->GetProperty()->SetColor(0.5,0.2,0.0);

    VTK_CREATE(vtkRenderer, ren1);
    ren1->AddActor( meshActor );
    ren1->AddActor( edgeActor );
    ren1->SetBackground( 0.1, 0.2, 0.1 );
    vtkPolyDataMapper::SetResolveCoincidentTopologyToPolygonOffset();
//    VTK_CREATE(vtkRenderWindow, renWin);
//    renWin->AddRenderer( ren1 );
//    renWin->SetSize( 300, 300 );

    this->ui->qvtkWidget->GetRenderWindow()->AddRenderer(ren1);

//    if (!_populatedVTKPolyData)
//    {
//        int st = PopulateVTKPolyData();
//        if (st != 0)
//        {

//        }
//        else
//        {
//            ShowMesh();
//        }
//    }
}

int Image2Mesh::PopulateVTKPolyData()
{

//    std::map<Vertex_handle, uint64_t> V;
//    uint64_t inum = 1;
//    VTK_CREATE(vtkPoints, points);

//    for( Vertices_iterator vit = tr.finite_vertices_begin();
//         vit != tr.finite_vertices_end();
//         ++vit)
//    {
//        V[vit] = inum++;
//        Point_3 p = vit->point();
//        points->InsertNextPoint(CGAL::to_double(p.x()),
//                                CGAL::to_double(p.y()),
//                                CGAL::to_double(p.z()));
//    }
//    VTK_CREATE(vtkCellArray,
}
