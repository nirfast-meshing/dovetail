#-------------------------------------------------
#
# Project created by QtCreator 2011-10-31T16:14:49
#
#-------------------------------------------------

QT       += core gui

TARGET = Dovetail
TEMPLATE = app

INCLUDEPATH += ../../../../../Desktop/Incoming/codes/MyMetaImageIO/MyMetaImageIO
INCLUDEPATH += ../../../../../local/CGAL/3.9/include

SOURCES += main.cpp\
        image2mesh.cpp \
    ../../../../../Desktop/Incoming/codes/MyMetaImageIO/MyMetaImageIO/metaimageio.cpp \
    cgalmeshgenerator.cpp

HEADERS  += image2mesh.h \
    ../../../../../Desktop/Incoming/codes/MyMetaImageIO/MyMetaImageIO/MyMetaImageIO_global.h \
    ../../../../../Desktop/Incoming/codes/MyMetaImageIO/MyMetaImageIO/metaimageio.h \
    cgalmeshgenerator.h \
    cgal_header.h

LIBS += /usr/local/lib/libmpfr.a /usr/local/lib/libgmp.a \
    /usr/local/lib/libboost_thread-mt.a \
    ../../../../../local/CGAL/3.9/lib/libCGAL.a \
    ../../../../../local/CGAL/3.9/lib/libCGAL_ImageIO.a \
    -framework OpenGL -lz


QMAKE_CXXFLAGS += -Wall -fno-common \
        -frounding-math -fno-strict-aliasing

FORMS    += image2mesh.ui









