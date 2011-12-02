#ifndef IMAGE2MESH_H
#define IMAGE2MESH_H

#include <QWidget>
#include <QFileInfo>
#include <QDir>
#include <QString>
#include <QDoubleValidator>

#include "metaimageio.h"
#include "cgalmeshgenerator.h"
#include "qimagestackreader.h"

namespace Ui {
    class Image2Mesh;
}

class Image2Mesh : public QWidget
{
    Q_OBJECT

public:
    explicit Image2Mesh(QWidget *parent = 0);
    MetaImageIO mi;
    CGALMeshGenerator mesher;
    ~Image2Mesh();


    std::string makeFileName(std::string, std::string);

private slots:
    void on_pushButton_BrowseImage_clicked();

    void on_pushButton_GenerateMesh_clicked();

    void on_lineEdit_infilename_returnPressed();

    void on_pushButton_ViewMesh_clicked();

    void _checkpixelsize();

    void on_checkBoxClip_stateChanged(int arg1);

    void on_verticalSliderClip_valueChanged(int value);

    void on_checkBoxColor_stateChanged(int arg1);

signals:


private:
    Ui::Image2Mesh *ui;

    QImageStackReader _picturestack;
    QString lastImageFile;
    bool imageDataLoaded;
    bool _populatedVTKPolyData;
    bool _vtkaxescreated;
    bool _successimageloading;
    bool _picturestacktype;

    void UpdateImageProperties();
    void UpdateMeshingCriteria();
    void GetImageProperties();
    void GetMeshCriteria();
    int Run_CGALMeshGenerator();

    void _loadImage(QString);
    void _loadPictureStack();
    int PopulateVTKPolyData();
    int ShowMesh();
    double _tetscale;
//    vtkUnstructuredGrid* _vtkuG;
    vtkSmartPointer<vtkUnstructuredGrid> _vtkuG;
//    vtkOrientationMarkerWidget *_vtkAxesWidget;
    vtkSmartPointer<vtkOrientationMarkerWidget> _vtkAxesWidget;
    vtkSmartPointer<vtkAxesActor> _vtkAxes;
};

#endif // IMAGE2MESH_H
