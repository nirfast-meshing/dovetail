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
    ui->lineEdit_SpecialSize1->setText("size");
    ui->lineEdit_SpecialID1->setText("0");

    // Meshing settings
    ui->lineEdit_TetSize->setText("3.0");
    ui->lineEdit_TetQual->setText("3.0");
    ui->lineEdit_FacetAngle->setText("25.0");
    ui->lineEdit_FacetDistance->setText("3.0");
    ui->lineEdit_FacetSize->setText("3.0");

    ui->textEdit_StatusInfo->setReadOnly(true);
    _populatedVTKPolyData = false;
    _vtkaxescreated = false;
    imageDataLoaded = false;
    _picturestacktype = false;
    _tetscale = 1.6;
    _successimageloading = false;

    connect(ui->lineEdit_X, SIGNAL(textChanged(QString)), this, SLOT(_checkpixelsize()));
    connect(ui->lineEdit_Y, SIGNAL(textChanged(QString)), this, SLOT(_checkpixelsize()));
    connect(ui->lineEdit_Z, SIGNAL(textChanged(QString)), this, SLOT(_checkpixelsize()));

//    connect(this, SIGNAL(UpdateImageProperties(QString)), ui->lineEdit_SDfilename, SLOT(setText(QString)));
}

Image2Mesh::~Image2Mesh()
{
    delete ui;
    if (_vtkuG)
        _vtkuG->Delete();
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
    ui->lineEdit_TetSize->setText(QString::number(*mind * _tetscale,'f',4));
    ui->lineEdit_FacetSize->setText(QString::number(*mind * _tetscale,'f',4));

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

//    if (_picturestacktype)
//        _loadPictureStack();

    if (_successimageloading == false)
    {
        ui->textEdit_StatusInfo->clear();
        ui->textEdit_StatusInfo->insertPlainText("No image stack is loaded");
        ui->pushButton_ViewMesh->setEnabled(false);
        return;
    }
    GetImageProperties();
    GetMeshCriteria();

    if (Run_CGALMeshGenerator() != 0)
        ui->textEdit_StatusInfo->setText("\nMesh generation failed!");
    else
    {
        ui->textEdit_StatusInfo->clear();
        QString foo = "\nMesh generation completed.";
        foo += "\n  No of nodes: " + QString::number(mesher.NoOfVertices());
        foo += "\n  No of elements: " + QString::number(mesher.NoOfCells());
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
}

void Image2Mesh::_loadImage(QString imageFile)
{
    _successimageloading = false;
    QString _suffix = QFileInfo(imageFile).suffix();
    if (! imageFile.isEmpty() ) {
            if ( _suffix == "mha")
            {
                ui->lineEdit_infilename->setText(imageFile);
                mi.myheader = MetaHeader();
                mi._filename = imageFile.toStdString();
                mi.inrFilename = "";

                ui->pushButton_GenerateMesh->setEnabled(false);
                ui->pushButton_ViewMesh->setEnabled(false);
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
                    return;
                }

                if (mi.myheader.elementsize[2] == 1)
                {
                    ui->textEdit_StatusInfo->setText("Input data contains only 1 slice!\nCan't create mesh!");
                    ui->textEdit_RegionInfo->clear();
                    UpdateImageProperties();

                    QString lineEditStyle("QLineEdit {background: red;}");
                    ui->lineEdit_Slices->setStyleSheet(lineEditStyle);

                    ui->lineEdit_outputfilename->clear();
                    ui->pushButton_GenerateMesh->setEnabled(false);
                    _successimageloading = false;
                }
                else
                {
                    ui->lineEdit_Slices->setStyleSheet(styleSheet());
                    mesher.outFilename = makeFileName(mi._filename,std::string(".mesh"));
                    ui->lineEdit_outputfilename->setText(QString::fromStdString(mesher.outFilename));

                    UpdateImageProperties();
                    UpdateMeshingCriteria();
                    ui->pushButton_GenerateMesh->setEnabled(true);
                    _picturestacktype = false;
                    _successimageloading = true;
                }
            }
            else if (_suffix == "bmp" || _suffix == "png")
            {
                _picturestack.setFileNameRange(imageFile);
                if (!_picturestack.IsOk())
                {
                    std::cerr << " Could read stack of 2D images starting with file: " <<
                                 imageFile.toStdString() << std::endl;
                    QMessageBox::information(0,"error",QString(" Could not read file: ") +
                                             imageFile);
                    ui->textEdit_StatusInfo->insertPlainText("Could read stack of 2D images starting with file: " +
                                                             imageFile);
                    _successimageloading = false;
                    return;
                }
                else
                {
                    ui->lineEdit_Rows->setText(QString::number(_picturestack.Height()));
                    ui->lineEdit_Cols->setText(QString::number(_picturestack.Width()));
                    ui->lineEdit_Slices->setText(QString::number(_picturestack.Slices()));

                    ui->lineEdit_infilename->setText(_picturestack.Basename());

                    mesher.outFilename = makeFileName(imageFile.toStdString(),std::string(".mesh"));
                    ui->lineEdit_outputfilename->setText(QString::fromStdString(mesher.outFilename));

                    _loadPictureStack();
                    if (!_successimageloading)
                    {
                        std::cerr << " Could read stack of 2D images starting with file: " <<
                                     imageFile.toStdString() << std::endl;
                        QMessageBox::information(0,"error",QString(" Could not read file: ") +
                                                 imageFile);
                        ui->textEdit_StatusInfo->insertPlainText("Could read stack of 2D images starting with file: " +
                                                                 imageFile);
                    }

                    QString lineEditStyle("QLineEdit {background: red;}");
                    ui->lineEdit_X->setStyleSheet(lineEditStyle);
                    ui->lineEdit_Y->setStyleSheet(lineEditStyle);
                    ui->lineEdit_Z->setStyleSheet(lineEditStyle);

                    _picturestacktype = true;
                }
            }
            this->lastImageFile = imageFile;
        }
        else
            ui->lineEdit_infilename->clear();
}

void Image2Mesh::on_pushButton_ViewMesh_clicked()
{

    vtkPolyDataMapper::SetResolveCoincidentTopologyToPolygonOffset();
    ui->tabWidget->setCurrentIndex(1);
    VTK_CREATE(vtkGeometryFilter, geomFilter);
    geomFilter->AddInput(_vtkuG);

    VTK_CREATE(vtkPolyDataMapper, elementsMapper);
    elementsMapper->SetInput(geomFilter->GetOutput());

    VTK_CREATE(vtkActor, elementsActor);
    elementsActor->SetMapper(elementsMapper);

    VTK_CREATE(vtkProperty, elementsProp);
    elementsProp = elementsActor->GetProperty();
    elementsProp->SetColor(0.1,0.27,0.75);
    elementsProp->SetDiffuse(0);
    elementsProp->SetAmbient(1);
    elementsProp->SetInterpolationToFlat();

    VTK_CREATE(vtkExtractEdges, edgesFilter);
    edgesFilter->SetInput(geomFilter->GetOutput());

    VTK_CREATE(vtkPolyDataMapper, edgesMapper);
    edgesMapper->SetInput(edgesFilter->GetOutput());
    edgesMapper->ScalarVisibilityOff();

    VTK_CREATE(vtkActor,edgesActor);
    edgesActor->SetMapper(edgesMapper);

    VTK_CREATE(vtkProperty, edgesProp);
    edgesProp = edgesActor->GetProperty();
    edgesProp->SetColor(0.75,0.75,0.75);
    edgesProp->SetDiffuse(0);
    edgesProp->SetAmbient(1);
    edgesProp->SetLineWidth(1);

    VTK_CREATE(vtkRenderer, ren1);

    ren1->SetBackground( 0.1, 0.1, 0.1 );
    ren1->AddActor(elementsActor);
    ren1->AddActor(edgesActor);

    this->ui->qvtkWidget->GetRenderWindow()->AddRenderer(ren1);

    if (_vtkaxescreated)
    {
        _vtkAxesWidget->SetEnabled(0);
    }

//    _vtkAxesWidget = vtkOrientationMarkerWidget::New(); _vtkaxescreated = true;
    _vtkAxesWidget = vtkSmartPointer<vtkOrientationMarkerWidget>::New();_vtkaxescreated = true;
    vtkRenderWindowInteractor *iren = ren1->GetRenderWindow()->GetInteractor();
    _vtkAxesWidget->SetDefaultRenderer(ren1);
    _vtkAxesWidget->SetInteractor(iren);

//    VTK_CREATE(vtkAxesActor, _vtkAxes);
    _vtkAxes = vtkSmartPointer<vtkAxesActor>::New();
    _vtkAxesWidget->SetOrientationMarker(_vtkAxes);

    _vtkAxesWidget->SetOutlineColor( 0.9300, 0.5700, 0.1300 );
    _vtkAxesWidget->SetViewport( 0.0, 0.0, 0.34, 0.34 );
    _vtkAxesWidget->SetEnabled(1);
    _vtkAxesWidget->InteractiveOn();

    ren1->ResetCamera();

}

int Image2Mesh::PopulateVTKPolyData()
{
    if (_populatedVTKPolyData)
        _vtkuG->Delete();
    _vtkuG = 0;
    _vtkuG = CGAL::output_c3t3_to_vtk_unstructured_grid<C3t3>(this->mesher.c3t3, _vtkuG);
    if (!_vtkuG)
    {
        _populatedVTKPolyData = false;
        return 1;
    }
    else
    {
        _populatedVTKPolyData = true;
        return 0;
    }
}

void Image2Mesh::_checkpixelsize()
{
    double foo;
    bool _f = false;

    setUpdatesEnabled(false);

    foo = ui->lineEdit_X->text().toDouble();
    if (foo>0)
    {
        _f = true;
        ui->lineEdit_X->setStyleSheet(styleSheet());
    }
    else
        _f = false;

    foo = ui->lineEdit_Y->text().toDouble();
    if (foo>0)
    {
        _f = _f && true;
        ui->lineEdit_Y->setStyleSheet(styleSheet());
    }
    else
        _f = false;

    foo = ui->lineEdit_Z->text().toDouble();
    if (foo>0)
    {
        _f = _f && true;
        ui->lineEdit_Z->setStyleSheet(styleSheet());
    }
    else
        _f = false;

    if (_f)
        ui->pushButton_GenerateMesh->setEnabled(true);

    setUpdatesEnabled(true);

}

void Image2Mesh::_loadPictureStack()
{
    QByteArray *data = _picturestack.readstack();
    if (!_picturestack.IsOk())
    {
        _successimageloading = false;
        return;
    }
    else
    {
        mi = MetaImageIO(data);
        double _ps;
        if (std::fabs(_picturestack.PixelSize() - 0) < 1e-6)
            _ps = 0.1;
        else
            _ps = _picturestack.PixelSize();
        mi.myheader.elementspacing[0] = _ps;
        mi.myheader.elementspacing[1] = _ps;
        mi.myheader.elementspacing[2] = _ps;
        mi.myheader.offset[0] = 0.;
        mi.myheader.offset[1] = 0.;
        mi.myheader.offset[2] = 0.;
        mi.myheader.elementsize = mi.myheader.elementspacing;
        mi.myheader.compresseddata = false;
        mi.myheader.elementtype = "uchar";
        mi.myheader.bitdepth = 8;
        mi.myheader.dimsize[0] = _picturestack.Width();
        mi.myheader.dimsize[1] = _picturestack.Height();
        mi.myheader.dimsize[2] = _picturestack.Slices();
        mi.inrFilename = _picturestack.Basename().toStdString() + ".inr";
        mi.SetDataSize(mi.myheader.dimsize[0]*mi.myheader.dimsize[1]*mi.myheader.dimsize[2]);
        mi.myheader.ndims = 3;
        mi.myheader.byteordermsb = false;
        mi._GetImageLabels();

        UpdateImageProperties();
        UpdateMeshingCriteria();
        this->_successimageloading = true;
    }
}
