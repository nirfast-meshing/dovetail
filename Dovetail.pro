#-------------------------------------------------
#
# Project created by QtCreator 2011-10-31T16:14:49
#
#-------------------------------------------------

QT       += core gui

TARGET = Dovetail
TEMPLATE = app

INCLUDEPATH += /Users/hamid_r_ghadyani/local/CGAL/3.8/include
INCLUDEPATH += ../MyMetaImageIO/MyMetaImageIO

SOURCES += main.cpp\
        image2mesh.cpp \
        cgalmeshgenerator.cpp \
    	../MyMetaImageIO/MyMetaImageIO/metaimageio.cpp
    	

HEADERS  += image2mesh.h \
        cgalmeshgenerator.h \
        cgal_header.h \
    	../MyMetaImageIO/MyMetaImageIO/MyMetaImageIO_global.h \
    	../MyMetaImageIO/MyMetaImageIO/metaimageio.h

LIBS += /usr/local/lib/libmpfr.a /usr/local/lib/libgmp.a \
    	/usr/local/lib/libboost_thread-mt.a \
        /Users/hamid_r_ghadyani/local/CGAL/3.8/lib/libCGAL.a \
        /Users/hamid_r_ghadyani/local/CGAL/3.8/lib/libCGAL_ImageIO.a \
    	-framework OpenGL -lz


QMAKE_CXXFLAGS += -Wall -fno-common \
        		  -frounding-math -fno-strict-aliasing

FORMS    += image2mesh.ui









