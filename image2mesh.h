#ifndef IMAGE2MESH_H
#define IMAGE2MESH_H

#include <QWidget>
#include <QFileInfo>
#include <QDir>
#include <QString>
#include "metaimageio.h"

namespace Ui {
    class Image2Mesh;
}

class Image2Mesh : public QWidget
{
    Q_OBJECT

public:
    explicit Image2Mesh(QWidget *parent = 0);
    ~Image2Mesh();

private slots:
    void on_pushButton_BrowseImage_clicked();

signals:


private:
    Ui::Image2Mesh *ui;
    MetaImageIO mi;
    QString lastImageFile;

    void UpdateImageProperties();
};

#endif // IMAGE2MESH_H
