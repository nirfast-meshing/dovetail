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

//    void on_checkBoxColor_stateChanged(int arg1);

    void on_checkBoxBadQuality_toggled(bool checked);

    void on_lineEditQualThreshold_returnPressed();

    void on_radioButtonCollapse_toggled(bool checked);

    void on_checkBoxColor_toggled(bool checked);

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
    double _tetscale;
    double *_bbx;
    double *_center;
    bool _1sttime_chart;
    int currqidx;
    double qthreshold;
    std::vector<uint32_t> scalefactor;
    std::vector<double> minq;
    std::vector<double> avgq;
    std::vector<double> maxq;
    std::vector<double> stdq;
    //    vtkUnstructuredGrid* _vtkuG;

    void UpdateImageProperties();
    void UpdateMeshingCriteria();
    void GetImageProperties();
    void GetMeshCriteria();
    int Run_CGALMeshGenerator();

    void _loadImage(QString);
    void _loadPictureStack();
    int PopulateVTKPolyData();
    int ShowMesh();
    void do_clip();
//    void update_mesh();
    void charttypetoggled(bool);
    void initchart();
    double compute_scalefactor(int);
    void drawchart(int chtype);
    void init_clip();
    void init_qfilter();
    void update_qfilter();
    void initmesh();
    void UpdateView();

    vtkSmartPointer<vtkUnstructuredGrid> _vtkuG;
    vtkSmartPointer<vtkOrientationMarkerWidget> _vtkAxesWidget;
    vtkSmartPointer<vtkAxesActor> _vtkAxes;

    vtkSmartPointer<vtkGeometryFilter> _gfilter;
    vtkSmartPointer<vtkGeometryFilter> _gf2;
    vtkSmartPointer<vtkGeometryFilter> _gf;
    vtkSmartPointer<vtkGeometryFilter> _gf1;

    vtkSmartPointer<vtkPolyDataMapper> cellband_mapper;
    vtkSmartPointer<vtkPolyDataMapper> visible_mapper;
    vtkSmartPointer<vtkPolyDataMapper> celledge_mapper;
    vtkSmartPointer<vtkPolyDataMapper> edge_mapper;
    vtkSmartPointer<vtkDataSetMapper> qcell_mapper;

    vtkSmartPointer<vtkExtractEdges> celledges;
    vtkSmartPointer<vtkExtractEdges> edges;

    vtkSmartPointer<vtkActor> celledgeActor;
    vtkSmartPointer<vtkActor> cellActor;
    vtkSmartPointer<vtkActor> qcellActor;
    vtkSmartPointer<vtkActor> visibleedgeActor;
    vtkSmartPointer<vtkActor> visibleactor;

    vtkSmartPointer<vtkRenderer> _ren;
    vtkSmartPointer<vtkAlgorithmOutput> _visibleport;
    vtkSmartPointer<vtkPlane> _cutplane;
    vtkSmartPointer<vtkExtractGeometry> _eg;

    vtkSmartPointer<vtkChartXY> plot;
    vtkSmartPointer<vtkContextView> view;
    vtkSmartPointer<vtkMeshQuality> quality;

    vtkSmartPointer<vtkDoubleArray> qvolarray;
    vtkSmartPointer<vtkDoubleArray> qanglearray;

    vtkSmartPointer<vtkImageData> vtkimage;
    vtkSmartPointer<vtkImageExtractComponents> extract;
    vtkSmartPointer<vtkImageAccumulate> histo;

    vtkSmartPointer<vtkTable> table;
    vtkSmartPointer<vtkMeshQuality> bandcellq;
    vtkSmartPointer<vtkThreshold> selectcells;

    vtkSmartPointer<vtkClipPolyData> clip;



};

#endif // IMAGE2MESH_H
