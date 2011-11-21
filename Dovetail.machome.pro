#-------------------------------------------------
#
# Project created by QtCreator 2011-10-31T16:14:49
#
#-------------------------------------------------

QT       += core gui 
#CONFIG   += debug

TARGET = Dovetail
TEMPLATE = app

INCLUDEPATH += /Volumes/Home/foo/local/CGAL/3.9/include
INCLUDEPATH += ../MyMetaImageIO/MyMetaImageIO
INCLUDEPATH += ../include

SOURCES += main.cpp\
        image2mesh.cpp \
        cgalmeshgenerator.cpp \
    	../MyMetaImageIO/MyMetaImageIO/metaimageio.cpp \
    ../include/CStopWatch.cpp
    	

HEADERS  += image2mesh.h \
        cgalmeshgenerator.h \
        cgal_header.h \
    	../MyMetaImageIO/MyMetaImageIO/MyMetaImageIO_global.h \
    	../MyMetaImageIO/MyMetaImageIO/metaimageio.h \
    ../include/CStopWatch.h

LIBS += /usr/local/lib/libmpfr.a /usr/local/lib/libgmp.a \
    	/usr/local/lib/libboost_thread-mt.a \
        /Volumes/Home/foo/local/CGAL/3.9/lib/libCGAL.a \
        /Volumes/Home/foo/local/CGAL/3.9/lib/libCGAL_ImageIO.a \
    	-framework OpenGL -lz


QMAKE_CXXFLAGS += -Wall -fno-common \
        		  -frounding-math -fno-strict-aliasing

FORMS    += image2mesh.ui











