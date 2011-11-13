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
//    connect(this, SIGNAL(UpdateImageProperties(QString)), ui->lineEdit_SDfilename, SLOT(setText(QString)));
}

Image2Mesh::~Image2Mesh()
{
    delete ui;
}

void Image2Mesh::on_pushButton_BrowseImage_clicked()
{
    const QString imageFile = QFileDialog::getOpenFileName(this, tr("Select Image file(s)"),
                                                     "",tr("Images (*.bmp *.tiff *.png *.mha)"),
                                                     NULL, QFileDialog::ReadOnly);
    if (! imageFile.isEmpty() ) {
        if (QFileInfo(imageFile).suffix() == "mha")
        {
            ui->lineEdit_infilename->setText(imageFile);
            mi._filename = imageFile.toStdString();
            if (mi.ReadHeader() == 1)
            {
                std::cerr << " Could not read " << imageFile.toStdString() << std::endl;
                ui->lineEdit_infilename->setText(QString("Could not read file!"));
            }
            ui->pushButton_GenerateMesh->setEnabled(true);
            UpdateImageProperties();
        }

//        emit
        this->lastImageFile = imageFile;
    }
    else
        ui->lineEdit_infilename->clear();
}

void Image2Mesh::UpdateImageProperties()
{
    char s[512];
    sprintf(s,"%d",mi.myheader.dimsize[0]);
    ui->lineEdit_Rows->setText(QString(s));

    sprintf(s,"%d",mi.myheader.dimsize[1]);
    ui->lineEdit_Cols->setText(QString(s));

    sprintf(s,"%d",mi.myheader.dimsize[2]);
    ui->lineEdit_Slices->setText(QString(s));

    sprintf(s,"%.3f",mi.myheader.elementsize[0]);
    ui->lineEdit_X->setText(s);

    sprintf(s,"%.3f",mi.myheader.elementsize[1]);
    ui->lineEdit_Y->setText(s);

    sprintf(s,"%.3f",mi.myheader.elementsize[2]);
    ui->lineEdit_Z->setText(s);

    sprintf(s,"%.2f",mi.myheader.offset[0]);
    ui->lineEdit_OffsetX->setText(s);

    sprintf(s,"%.2f",mi.myheader.offset[1]);
    ui->lineEdit_OffsetY->setText(s);

    sprintf(s,"%.2f",mi.myheader.offset[2]);
    ui->lineEdit_OffsetZ->setText(s);
}


void Image2Mesh::on_pushButton_GenerateMesh_clicked()
{
    // TODO
    // Get the output file name and construct corresponding
    // inr filename and set inrFilename before calling
    // CGALMeshGenerator
    if (Run_CGALMeshGenerator(mi) != 0)
        ui->lineEdit_infilename->setText("Error in Mesh Generator");
}

int Run_CGALMeshGenerator(MetaImageIO& _mi)
{
    CGALMeshGenerator generator(_mi);
    if (generator.metaimage_error)
        return 1;
    else
    {
        if (generator.Execute() != 0)
            return 1; // Unsuccessful
        else
            return 0;
    }

}
