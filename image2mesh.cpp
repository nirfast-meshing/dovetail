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
    ui->tabWidget->setCurrentIndex(0);
    ui->verticalSliderClip->setEnabled(false);
    ui->checkBoxColor->setEnabled(true);

    // Meshing settings
    ui->lineEdit_TetSize->setText("3.0");
    ui->lineEdit_TetQual->setText("3.0");
    ui->lineEdit_FacetAngle->setText("25.0");
    ui->lineEdit_FacetDistance->setText("3.0");
    ui->lineEdit_FacetSize->setText("3.0");

    ui->textEdit_StatusInfo->setReadOnly(true);
    ui->plainTextQualityStats->setReadOnly(true);

    ui->tab_MeshViewer->setEnabled(false);
    ui->tab_QualityChart->setEnabled(false);

    _populatedVTKPolyData = false;
    _vtkaxescreated = false;
    imageDataLoaded = false;
    _picturestacktype = false;
    _tetscale = 1.6;
    _successimageloading = false;

    _bbx = new double[6];
    _center = new double[3];

    connect(ui->lineEdit_X, SIGNAL(textChanged(QString)), this, SLOT(_checkpixelsize()));
    connect(ui->lineEdit_Y, SIGNAL(textChanged(QString)), this, SLOT(_checkpixelsize()));
    connect(ui->lineEdit_Z, SIGNAL(textChanged(QString)), this, SLOT(_checkpixelsize()));

    ui->checkBoxBadQuality->setEnabled(0);
    ui->lineEditQualThreshold->setEnabled(0);
//    ui->radioButtonVolume->setChecked(true);
    _1sttime_chart = true;
    scalefactor.resize(2,0);
    qthreshold.resize(2,0);
}

Image2Mesh::~Image2Mesh()
{
    try
    {
//        delete _bbx;
//        delete _center;
    }
    catch(std::exception &e)
    {
        std::cout << "Standard Exception: " << e.what() << std::endl;
    }

    delete ui;
//    if (_vtkuG)
//        _vtkuG->Delete();
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
    ui->tabWidget->setCurrentIndex(1);
    ShowMesh();
}

int Image2Mesh::ShowMesh()
{

    if (_vtkaxescreated)
    {
        _vtkAxesWidget->SetEnabled(0);
    }

    VTK_NEW(vtkOrientationMarkerWidget, _vtkAxesWidget);
    _vtkaxescreated = true;
    vtkRenderWindowInteractor *iren = _ren->GetRenderWindow()->GetInteractor();
    _vtkAxesWidget->SetDefaultRenderer(_ren);
    _vtkAxesWidget->SetInteractor(iren);

    _vtkAxes = vtkSmartPointer<vtkAxesActor>::New();
    _vtkAxesWidget->SetOrientationMarker(_vtkAxes);

    _vtkAxesWidget->SetOutlineColor( 0.9300, 0.5700, 0.1300 );
    _vtkAxesWidget->SetViewport( 0.0, 0.0, 0.34, 0.34 );
    _vtkAxesWidget->SetEnabled(1);
    _vtkAxesWidget->InteractiveOn();

    return 0;
//    ren1->ResetCamera();
}

int Image2Mesh::PopulateVTKPolyData()
{
//    if (_populatedVTKPolyData)
//        _vtkuG->Delete();
//    _vtkuG = 0;
    _vtkuG = vtkSmartPointer<vtkUnstructuredGrid>::New();
    CGAL::output_c3t3_to_vtk_unstructured_grid<C3t3>(this->mesher.c3t3, _vtkuG);
    if (!_vtkuG)
    {
        _populatedVTKPolyData = false;
        return 1;
    }
    else
    {
        _populatedVTKPolyData = true;
        initmesh();
        initchart();
        drawchart(0);
        init_clip();
        init_qfilter();
        ui->radioButtonVolume->toggle();
        ui->tab_MeshViewer->setEnabled(true);
        ui->tab_QualityChart->setEnabled(true);
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

void Image2Mesh::on_checkBoxClip_stateChanged(int arg1)
{
    std::cerr << " checnaged ........ " << arg1 << '\n';
    if (arg1 == 0)
    {
        if (ui->checkBoxBadQuality->isChecked())
            _ren->RemoveActor(qcellActor);
        ui->verticalSliderClip->setEnabled(false);
        _visibleport = _gfilter->GetOutputPort();
        _ren->RemoveActor(cellActor);
        _ren->RemoveActor(celledgeActor);
        ui->checkBoxBadQuality->setEnabled(false);
        ui->lineEditQualThreshold->setEnabled(false);
        UpdateView();
    }
    else if (arg1 == 2)
    {
        _visibleport = _gf1->GetOutputPort();
        if (ui->checkBoxBadQuality->isChecked())
        {
            qthreshold[currqidx] = ui->lineEditQualThreshold->text().toDouble();
            update_qfilter();
            _ren->AddViewProp(qcellActor);
        }
        else
        {
            celledge_mapper->Update();
            _ren->AddViewProp(cellActor);
            _ren->AddViewProp(celledgeActor);
        }
        ui->verticalSliderClip->setEnabled(true);
        ui->checkBoxBadQuality->setEnabled(true);
        ui->lineEditQualThreshold->setEnabled(true);
        UpdateView();
    }
}

void Image2Mesh::do_clip()
{
    if (ui->checkBoxBadQuality->isChecked())
        update_qfilter();
    UpdateView();
}


void Image2Mesh::on_verticalSliderClip_valueChanged(int value)
{
    double origin[3] = {_center[0], _center[1], _center[2]};
    origin[2] = static_cast<double>(value)/100*(_bbx[5]-_bbx[4])+_bbx[4];
    _cutplane->SetOrigin(origin);
    do_clip();
}

void Image2Mesh::on_checkBoxColor_toggled(bool arg1)
{
    if (arg1 == 2)
    {
        edge_mapper->SetScalarModeToUseCellData();
        celledge_mapper->SetScalarModeToUseCellData();
        visible_mapper->SetScalarModeToUseCellData();
    }
    else if (arg1 == 0)
    {
        edge_mapper->SetScalarModeToUsePointData();
        celledge_mapper->SetScalarModeToUsePointData();
        visible_mapper->SetScalarModeToUsePointData();
    }
    UpdateView();
}

void Image2Mesh::initchart()
{
    plot = vtkSmartPointer<vtkChartXY>::New();
    view = vtkSmartPointer<vtkContextView>::New();
    view->SetInteractor(ui->qvtkChart->GetInteractor());
    ui->qvtkChart->SetRenderWindow(view->GetRenderWindow());
    view->GetScene()->AddItem(plot);

    quality =vtkSmartPointer<vtkMeshQuality>::New();
    quality->SetInput(_vtkuG);
    quality->SetTetQualityMeasureToVolume();
    quality->Update();

    minq.clear(); avgq.clear(); maxq.clear(); stdq.clear();

    minq.push_back(quality->GetOutput()->GetFieldData()->GetArray("Mesh Tetrahedron Quality")->GetComponent(0,0));
    avgq.push_back(quality->GetOutput()->GetFieldData()->GetArray("Mesh Tetrahedron Quality")->GetComponent(0,1));
    maxq.push_back(quality->GetOutput()->GetFieldData()->GetArray("Mesh Tetrahedron Quality")->GetComponent(0,2));
    stdq.push_back(quality->GetOutput()->GetFieldData()->GetArray("Mesh Tetrahedron Quality")->GetComponent(0,3));

    qvolarray = vtkDoubleArray::SafeDownCast(quality->GetOutput()->GetCellData()->GetArray("Quality"));
    std::cout << "qvolarray range: " << qvolarray->GetRange()[0] << ' ' << qvolarray->GetRange()[1] << '\n';
    std::cout.flush();
    scalefactor[0] = compute_scalefactor(0);
    if (scalefactor[0] != 1)
    {
        for (uint32_t i=0; i<qvolarray->GetNumberOfTuples(); ++i)
        {
            qvolarray->SetTuple1(i, qvolarray->GetTuple1(i) * scalefactor[0]);
        }
        minq[0] = minq[0] * scalefactor[0];
        maxq[0] = maxq[0] * scalefactor[0];
        avgq[0] = avgq[0] * scalefactor[0];
        stdq[0] = stdq[0] * scalefactor[0];
    }

    quality->SetTetQualityMeasureToMinAngle();
    quality->Update();

    minq.push_back(quality->GetOutput()->GetFieldData()->GetArray("Mesh Tetrahedron Quality")->GetComponent(0,0));
    avgq.push_back(quality->GetOutput()->GetFieldData()->GetArray("Mesh Tetrahedron Quality")->GetComponent(0,1));
    maxq.push_back(quality->GetOutput()->GetFieldData()->GetArray("Mesh Tetrahedron Quality")->GetComponent(0,2));
    stdq.push_back(quality->GetOutput()->GetFieldData()->GetArray("Mesh Tetrahedron Quality")->GetComponent(0,3));

    qanglearray = vtkDoubleArray::SafeDownCast(quality->GetOutput()->GetCellData()->GetArray("Quality"));
    std::cout << "qanglearray range: " << qanglearray->GetRange()[0] << ' ' << qanglearray->GetRange()[1] << '\n';
    std::cout.flush();
    scalefactor[1] = compute_scalefactor(1);
    if (scalefactor[1] != 1)
    {
        for (uint32_t i=0; i<qanglearray->GetNumberOfTuples(); ++i)
        {
            qanglearray->SetTuple1(i, qanglearray->GetTuple1(i) * scalefactor[1]);
        }
        minq[1] = minq[1] * scalefactor[1];
        maxq[1] = maxq[1] * scalefactor[1];
        avgq[1] = avgq[1] * scalefactor[1];
        stdq[1] = stdq[1] * scalefactor[1];
    }
    currqidx = 0;
}

void Image2Mesh::drawchart(int chtype)
{
    vtkSmartPointer<vtkDoubleArray> currarray;
    std::string qname;
    if (chtype == 0)
    {
        currarray = qvolarray;
        qname = std::string("Volume");
    }
    else
    {
        currarray = qanglearray;
        qname = std::string("Min. Dihedral Angle");
    }

//    view->GetScene()->RemoveItem(plot);
    view->SetInteractor(ui->qvtkChart->GetInteractor());
    ui->qvtkChart->SetRenderWindow(view->GetRenderWindow());


    vtkimage = vtkSmartPointer<vtkImageData>::New();
    vtkimage->SetDimensions(currarray->GetNumberOfTuples(), 1, 1);
    vtkimage->GetPointData()->SetScalars(currarray);
    vtkimage->SetNumberOfScalarComponents(1);
    vtkimage->SetScalarTypeToDouble();
    vtkimage->Update();

    extract = vtkSmartPointer<vtkImageExtractComponents>::New();
    extract->SetInput(vtkimage);
    extract->SetComponents(0);
    extract->Update();

    double *_range = extract->GetOutput()->GetScalarRange();
    int nbins = static_cast<int>(_range[1] - _range[0]) + 1;
    if (nbins > 50)
        nbins = 50;
    std::cout << "extract range is : ";
    std::cout << _range[0] << ' ' << _range[1] << ' ' << nbins << '\n';

    histo = vtkSmartPointer<vtkImageAccumulate>::New();
    histo->SetInputConnection(extract->GetOutputPort());
    histo->SetComponentExtent(0, nbins-1, 0, 0, 0, 0);
    histo->SetComponentOrigin(_range[0], 0, 0);
    histo->SetComponentSpacing((_range[1] - _range[0])/nbins, 0, 0);
    histo->SetIgnoreZero(0);
    histo->Update();

//    double xmax = _range[1];
//    double ymax = histo->GetOutput()->GetScalarRange()[1];

    VTK_CREATE(vtkDoubleArray, arrX);
    VTK_CREATE(vtkDoubleArray, arrC);

    arrX->SetName(qname.c_str());
    arrC->SetName("Y");

    for (uint32_t i=0; i<histo->GetOutput()->GetNumberOfPoints(); ++i)
    {
        arrX->InsertNextValue(histo->GetOutput()->GetPointData()->GetArray(0)->GetTuple1(i));
        arrC->InsertNextValue(histo->GetOutput()->GetPoint(i)[0]);
    }

    std::cout << arrX->GetRange()[0] << ' ' << arrX->GetRange()[1] << '\n';
    std::cout << arrC->GetRange()[0] << ' ' << arrC->GetRange()[1] << '\n';
    std::cout.flush();
    table = vtkSmartPointer<vtkTable>::New();
    table->AddColumn(arrX);
    table->AddColumn(arrC);

    if (_1sttime_chart == false)
        plot->ClearPlots();

    vtkSmartPointer<vtkPlot> line0 = plot->AddPlot(vtkChart::BAR);
    line0->SetInput(table, 1, 0);
    line0->SetColor(50,100,10,255);
    line0->SetWidth(3.0);
    line0->GetPen()->SetLineType(vtkPen::SOLID_LINE);

    plot->SetShowLegend(true);
    plot->SetTitle("Mehs Quality Histogram");
    plot->GetAxis(vtkAxis::LEFT)->SetTitle("Freq.");
    plot->GetAxis(vtkAxis::BOTTOM)->SetRange(arrX->GetRange()[0], arrX->GetRange()[1]);
    plot->Update();

    _1sttime_chart = false;

    std::string atext, aval;
    if (chtype == 0)
    {
        currqidx = 0;
        atext = "";
        aval = "";
    }
    else
    {
        currqidx = 1;
        atext = "Acceptable Range: 22.0 - 71.0";
        aval = "Ideal Valeu: 70.5";
    }
    plot->GetAxis(vtkAxis::BOTTOM)->SetTitle(qname + " x" + QString::number(1./scalefactor[currqidx], 'e',0).toStdString());
    ui->groupBox_6->setTitle(QString::fromAscii("Stats (x")
                             + QString::number(1./scalefactor[currqidx], 'e',0)
                             + QString::fromAscii(")"));
    QString text = "Min: " + QString::number(minq[currqidx],'f',2) + "\nAvg: ";
    text = text + QString::number(avgq[currqidx],'f',2) + "\nMax: ";
    text = text + QString::number(maxq[currqidx],'f',2) + "\nStd Dev: ";
    text = text + QString::number(stdq[currqidx],'f',2);
    text = text + "\n" + atext.c_str() + "\n" + aval.c_str();
    ui->plainTextQualityStats->setPlainText(text);
}

void Image2Mesh::init_clip()
{
    _eg = vtkSmartPointer<vtkExtractGeometry>::New();
    _eg->ExtractInsideOn();;
    _eg->ExtractBoundaryCellsOn();
    _eg->ExtractOnlyBoundaryCellsOn();
    _eg->SetInput(_vtkuG);
    _eg->SetImplicitFunction(_cutplane);
    _eg->Update();

    _gf2 = vtkSmartPointer<vtkGeometryFilter>::New();
    _gf2->AddInputConnection(_eg->GetOutputPort());

    cellband_mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    cellband_mapper->SetInputConnection(_gf2->GetOutputPort());
    cellband_mapper->SetScalarModeToUseCellData();
    cellband_mapper->SetScalarRange(_vtkuG->GetScalarRange());

    cellActor = vtkSmartPointer<vtkActor>::New();
    cellActor->SetMapper(cellband_mapper);

    celledges = vtkSmartPointer<vtkExtractEdges>::New();
    celledges->SetInputConnection(_gf2->GetOutputPort());

    celledge_mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    celledge_mapper->SetScalarModeToUseCellData();
    celledge_mapper->SetInputConnection(celledges->GetOutputPort());
    celledge_mapper->SetScalarRange(_vtkuG->GetScalarRange());

    celledgeActor = vtkSmartPointer<vtkActor>::New();
    celledgeActor->SetMapper(celledge_mapper);
    celledgeActor->GetProperty()->SetColor(0.2,0.3,0.4);

}

void Image2Mesh::init_qfilter()
{
    bandcellq = vtkSmartPointer<vtkMeshQuality>::New();
    bandcellq->SetInputConnection(_eg->GetOutputPort());
    if (ui->radioButtonVolume->isChecked())
        bandcellq->SetTetQualityMeasureToVolume();
    else
        bandcellq->SetTetQualityMeasureToMinAngle();

    selectcells = vtkSmartPointer<vtkThreshold>::New();
    selectcells->ThresholdByLower(qthreshold[currqidx]);
    selectcells->SetInputArrayToProcess(0,0,0,
                                        vtkDataObject::FIELD_ASSOCIATION_CELLS,
                                        vtkDataSetAttributes::SCALARS);
    selectcells->SetInputConnection(bandcellq->GetOutputPort());

    qcellActor = vtkSmartPointer<vtkActor>::New();
    qcell_mapper = vtkSmartPointer<vtkDataSetMapper>::New();

    qcell_mapper->SetInputConnection(selectcells->GetOutputPort());
    qcell_mapper->Update();

    qcellActor->SetMapper(qcell_mapper);
    qcellActor->GetProperty()->SetRepresentationToWireframe();
    qcellActor->GetProperty()->SetColor(1,0,0);
    qcellActor->GetProperty()->SetLineWidth(5);

}

void Image2Mesh::update_qfilter()
{
    if (ui->radioButtonVolume->isChecked())
    {
        bandcellq->SetTetQualityMeasureToVolume();
        bandcellq->Update();
    }
    else
    {
        bandcellq->SetTetQualityMeasureToMinAngle();
        bandcellq->Update();
    }

    selectcells->ThresholdByLower(qthreshold[currqidx]);
    qcell_mapper->Update();

}

void Image2Mesh::initmesh()
{
    vtkPolyDataMapper::SetResolveCoincidentTopologyToPolygonOffset();
    VTK_NEW(vtkGeometryFilter, this->_gfilter);
    _gfilter->AddInput(_vtkuG);

    VTK_NEW(vtkPolyDataMapper, visible_mapper);
    visible_mapper->SetScalarModeToUseCellData();
    visible_mapper->SetScalarModeToUsePointData();
    visible_mapper->SetScalarRange(_vtkuG->GetScalarRange());

    VTK_NEW(vtkExtractEdges, edges);
    VTK_NEW(vtkPolyDataMapper, edge_mapper);
    edge_mapper->SetScalarModeToUsePointData();
    edge_mapper->SetScalarRange(_vtkuG->GetScalarRange());

    VTK_NEW(vtkActor, visibleedgeActor);
    VTK_NEW(vtkActor, visibleactor);
    VTK_NEW(vtkRenderer, _ren);

    _visibleport = _gfilter->GetOutputPort();
    _ren->ResetCamera();

    VTK_NEW(vtkPlane, _cutplane);
    _cutplane->SetOrigin(_center);
    _cutplane->SetNormal(0.,0.,1.);

    _bbx = _vtkuG->GetBounds();
    _center = _vtkuG->GetCenter();

    VTK_NEW(vtkGeometryFilter, _gf);
    _gf->SetInput(_vtkuG);

    VTK_NEW(vtkClipPolyData, clip);
    clip->SetInputConnection(_gf->GetOutputPort());
    clip->SetClipFunction(_cutplane);
    clip->InsideOutOn();

    VTK_NEW(vtkGeometryFilter, _gf1);
    _gf1->AddInputConnection(clip->GetOutputPort());

    edge_mapper->SetInputConnection(edges->GetOutputPort());
    visibleedgeActor->SetMapper(edge_mapper);
    visibleedgeActor->GetProperty()->SetColor(0.2,0.3,0.40);

    visibleactor->SetMapper(visible_mapper);
    _ren->SetBackground(0.1,0.2,0.1);
    ui->qvtkWidget->GetRenderWindow()->AddRenderer(_ren);
    _ren->AddViewProp(visibleactor);
    _ren->AddViewProp(visibleedgeActor);

    UpdateView();

}


double Image2Mesh::compute_scalefactor(int i)
{
    double r = maxq[i] - minq[i];
    int c = 0;
    while (r < 10)
    {
        ++c;
        r *= std::pow(10., c);
    }
    return std::pow(10., c);
}

void Image2Mesh::on_radioButtonVolume_toggled(bool state)
{
    std::cout << "charte state: " << state << '\n';
    if (state == true)
    {
        currqidx = 0;

        if (ui->lineEditQualThreshold->text().isEmpty())
            qthreshold[currqidx] = avgq[currqidx] / scalefactor[currqidx];
        ui->lineEditQualThreshold->setText(QString::number(qthreshold[currqidx],'e',4));
        update_qfilter();
        drawchart(0); // volume quality
    }
    else
    {
        currqidx = 1;
        if (ui->lineEditQualThreshold->text().isEmpty())
            qthreshold[currqidx] = avgq[currqidx] / scalefactor[currqidx];
        ui->lineEditQualThreshold->setText(QString::number(qthreshold[currqidx],'e',4));
        update_qfilter();
        drawchart(1); // min diehedral angle
    }
}

void Image2Mesh::on_checkBoxBadQuality_toggled(bool checked)
{
    if (!checked)
    {
        _ren->RemoveActor(qcellActor);
        ui->lineEditQualThreshold->setEnabled(0);
        _ren->AddViewProp(cellActor);
        _ren->AddViewProp(celledgeActor);
    }
    else
    {
        ui->lineEditQualThreshold->setEnabled(true);
        if (ui->lineEditQualThreshold->text().isEmpty())
            ui->lineEditQualThreshold->setText(QString::number(qthreshold[currqidx],'e',4));
        qthreshold[currqidx] = ui->lineEditQualThreshold->text().toDouble();
        update_qfilter();
        _ren->AddViewProp(qcellActor);
        _ren->RemoveActor(cellActor);
        _ren->RemoveActor(celledgeActor);
    }
    UpdateView();
}

void Image2Mesh::on_lineEditQualThreshold_returnPressed()
{
    qthreshold[currqidx] = ui->lineEditQualThreshold->text().toDouble();
    update_qfilter();
    UpdateView();
}

void Image2Mesh::UpdateView()
{
    visible_mapper->SetInputConnection(_visibleport);
    edges->SetInputConnection(_visibleport);
    ui->qvtkWidget->GetRenderWindow()->Render();
}
