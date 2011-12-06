# -DQT_QMAKE_EXECUTABLE=/usr/local/bin/qmake -DVTK_DIR=/Users/hamid_r_ghadyani/local/VTK/nightly/120611/lib/vtk-5.9 -DCGAL_DIR=/Users/hamid_r_ghadyani/local/CGAL/3.8/lib/CGAL/ -DCMAKE_BUILD_TYPE=Release
# -DVTK_DIR=/Volumes/Home/foo/local/VTK/5.9.0/lib/vtk-5.9 -DQT_QMAKE_EXECUTABLE=/Volumes/Home/foo/local/Qt/4.7.4/bin/qmake -DCGAL_DIR=/Volumes/Home/foo/local/CGAL/3.9/lib/CGAL/
project(Dovetail)

cmake_minimum_required( VERSION 2.6 )
cmake_policy(SET CMP0017 NEW)
#set ( CMAKE_BUILD_TYPE Debug )

add_definitions ( -Wall -Wno-deprecated -fno-strict-aliasing ) # -Wl,-no-pie) # -mdynamic-no-pic

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)

IF(NOT VTK_BINARY_DIR)
FIND_PACKAGE(VTK)
IF(NOT VTK_DIR)
  MESSAGE(FATAL_ERROR "Please set VTK_DIR.")
ENDIF(NOT VTK_DIR)
INCLUDE(${VTK_USE_FILE})
ENDIF(NOT VTK_BINARY_DIR)


LIST(APPEND CMAKE_MODULE_PATH "/usr/local/lib/CGAL")
find_package ( Qt4 REQUIRED COMPONENTS QTCORE QTGUI)
include ( ${QT_USE_FILE} )
find_package (CGAL REQUIRED)
include ( ${CGAL_USE_FILE} )
find_package (Boost REQUIRED COMPONENTS thread)

# find_package (OpenGL REQUIRED)
# find_package (GMP REQUIRED)
# find_package (ZLIB REQUIRED)


include_directories (
	${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
	${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR}
	../MyMetaImageIO/MyMetaImageIO
	../header/)
set ( Dovetail_HDRS
	cgal_header.h
        cgal_tet_to_vtk.h
	cgalmeshgenerator.h
        qimagestackreader.h
	../MyMetaImageIO/MyMetaImageIO/MyMetaImageIO_global.h
	../MyMetaImageIO/MyMetaImageIO/metaimageio.h
	../header/CStopWatch.h
	)

set ( Dovetail_SRCS
	main.cpp
	image2mesh.cpp
        cgal_header.cpp # just a dummy
        cgal_tet_to_vtk.h # just a dummy
        cgalmeshgenerator.cpp
        qimagestackreader.cpp
	../MyMetaImageIO/MyMetaImageIO/metaimageio.cpp
	../header/CStopWatch.cpp
	)

set ( Dovetail_UIS
	image2mesh.ui
	)
QT4_WRAP_UI(UIS ${Dovetail_UIS})

set ( Dovetail_MOCS
	image2mesh.h
	)
QT4_WRAP_CPP(MOCS ${Dovetail_MOCS})

set(OsName ${CMAKE_SYSTEM_NAME})

set(QVTK_ QVTK
  vtkRendering
  vtkGraphics
  vtkIO
  vtkCommon
  vtkInfovis
  vtkViews
  vtkCharts
)

# QT4
set (QTLIBS_ ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
#CGAL
set (CGALLIBS_ ${Boost_LIBRARIES} ${CGAL_Core_LIBRARY} ${CGAL_ImageIO_LIBRARY}
	${CGAL_ImageIO_3RD_PARTY_LIBRARIES})
set (_SOURCES ${Dovetail_SRCS} ${UIS} ${RSCS} ${TRS} ${MOCS})
IF(${OsName} MATCHES "Linux")
  MESSAGE("THIS IS LINUX")
  set (OPENGL_ ${OPENGL_gl_LIBRARY} ${ZLIB_LIBRARIES})
  set (LIBS_ ${CGALLIBS_} ${QTLIBS_} ${QVTK_} ${OPENGL})

  ADD_EXECUTABLE(Dovetail ${_SOURCES})
  target_link_libraries ( Dovetail ${LIBS_})

ELSEIF(${OsName} MATCHES "Darwin")
  MESSAGE("THIS IS APPLE")
  set (LIBS_ ${CGALLIBS_} ${QTLIBS_} ${QVTK_})

  ADD_EXECUTABLE(Dovetail MACOSX_BUNDLE ${_SOURCES})
  target_link_libraries ( Dovetail ${LIBS_})

ELSEIF(${OsName} MATCHES "Windows")
  MESSAGE("THIS IS WINDOWS")
  set (LIBS_ ${CGALLIBS_} ${QTLIBS_} ${QVTK_} ${OPENGL})

  ADD_EXECUTABLE(Dovetail ${_SOURCES})
  target_link_libraries ( Dovetail ${LIBS_})
ENDIF()


# OpenGL
# set (OPENGL_ ${OPENGL_gl_LIBRARY} ${ZLIB_LIBRARIES})








