#-------------------------------------------------
#
# Project created by QtCreator 2015-02-12T21:22:31
#
#-------------------------------------------------

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = objview
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    trackball.cpp \
    transform.cpp \
    objparser.cpp \
    objmesh.cpp \
    viewer.cpp \
    filereader.cpp \
    optionedit.cpp \
    floor.cpp \
    camera.cpp \
    directionlight.cpp \
    staticlightoptionedit.cpp \
    colorbutton.cpp \
    dynamiclightoptionedit.cpp

HEADERS  += mainwindow.h \
    trackball.h \
    transform.h \
    objmesh.h \
    objparser.h \
    viewer.h \
    filereader.h \
    optionedit.h \
    floor.h \
    camera.h \
    directionlight.h \
    staticlightoptionedit.h \
    colorbutton.h \
    dynamiclightoptionedit.h

DISTFILES += \
    shader/simple.vert \
    todo.txt \
    floor.png \
    floor.frag \
    floor.vert

RESOURCES += \
    shader.qrc

FORMS += \
    mainwindow.ui

macx: LIBS += -L$$PWD/../../../../../Downloads/libQGLViewer-2.6.1/build-QGLViewer-Desktop_Qt_5_4_0_clang_64bit-Release/ -lQGLViewer

INCLUDEPATH += $$PWD/../../../../../Downloads/libQGLViewer-2.6.1/QGLViewer
DEPENDPATH += $$PWD/../../../../../Downloads/libQGLViewer-2.6.1/QGLViewer
