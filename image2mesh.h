#ifndef IMAGE2MESH_H
#define IMAGE2MESH_H

#include <QWidget>

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
    void UpdateImageProperties(const QString &);

private:
    Ui::Image2Mesh *ui;
    QString lastImageFile;
};

#endif // IMAGE2MESH_H
