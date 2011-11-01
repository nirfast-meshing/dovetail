#include <QtGui/QApplication>
#include "image2mesh.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Image2Mesh w;
    w.show();

    return a.exec();
}
