#ifndef IMAGE2MESH_H
#define IMAGE2MESH_H

#include <QWidget>
#include <QFileInfo>
#include <QDir>
#include <QString>
#include "metaimageio.h"
#include "cgalmeshgenerator.h"

namespace Ui {
    class Image2Mesh;
}

class Image2Mesh : public QWidget
{
    Q_OBJECT

public:
    explicit Image2Mesh(QWidget *parent = 0);
    MetaImageIO mi;
    ~Image2Mesh();

private slots:
    void on_pushButton_BrowseImage_clicked();

    void on_pushButton_GenerateMesh_clicked();

signals:


private:
    Ui::Image2Mesh *ui;

    QString lastImageFile;
    bool imageDataLoaded;

    void UpdateImageProperties();
};

#endif // IMAGE2MESH_H
