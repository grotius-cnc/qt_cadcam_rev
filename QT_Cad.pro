#-------------------------------------------------
#
# Project created by QtCreator 2020-03-20T09:39:04
#
#-------------------------------------------------

QT       += core gui opengl svg printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QT_Cad
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++20

unix: LIBS += -lglut -lGLU

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    opengl.cpp \
    variable.cpp

HEADERS += \
    cam/cam_gcode.h \
    cam/gravity_nester.h \
    display/zoom_extend.h \
    gui/add_template_files.h \
    gui/load_gcode_settings.h \
    gui/load_heads.h \
    gui/load_tools.h \
    mainwindow.h \
    opengl.h \
    display/input_data.h \
    structs.h \
    cam/cam_offset.h \
    cam/cam_contours.h \
    cam/cam_dept_sequence.h \
    dxf/dxf_read_arc.h \
    dxf/dxf_read_circle.h \
    dxf/dxf_read_dimension.h \
    dxf/dxf_read_ellipse.h \
    dxf/dxf_read_line.h \
    dxf/dxf_read_spline.h \
    dxf/dxf_read_text.h \
    lib/lib_intersect.h \
    lib/lib_matrix.h \
    lib/lib_offset.h \
    display/display.h \
    lib/lib_split.h \
    lib/lib_pocket.h \
    lib/lib_connect.h \
    lib/lib_convert.h \
    variable.h

FORMS += \
        mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    icons.qrc

DISTFILES += \
    trash.txt
