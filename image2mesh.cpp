#include "image2mesh.h"
#include "ui_image2mesh.h"
#include <QtGui>

Image2Mesh::Image2Mesh(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Image2Mesh)
{
    ui->setupUi(this);
    connect(this, SIGNAL(UpdateImageProperties(QString)), ui->lineEdit_SDfilename, SLOT(setText(QString)));
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
    if (! imageFile.isEmpty() ) {
        ui->lineEdit_infilename->setText(imageFile);
        emit UpdateImageProperties(imageFile);
        this->lastImageFile = imageFile;
    }
    else
        ui->lineEdit_infilename->clear();
}
