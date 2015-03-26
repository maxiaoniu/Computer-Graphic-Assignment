#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "viewer.h"
#include "objmesh.h"
#include "objparser.h"
#include "filereader.h"
#include "staticlightoptionedit.h"
#include <QFileDialog>
#include <QDebug>
#include <QFile>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //ui->glview = new Viewer();
    //ui->filereader = new FileReader(ui->groupBox_3);
    connect(ui->filereader,&FileReader::fileChanged,
            ui->glview,&Viewer::recFile);
    connect(ui->optionedit,&OptionEdit::colorChanged,
            ui->glview,&Viewer::setColorParameter);
    connect(ui->optionedit,&OptionEdit::rendermodeChanged,
            ui->glview,&Viewer::setRenderMode);
    connect(ui->optionedit,&OptionEdit::oritationChanged,
            ui->glview,&Viewer::setFaceOritation);
    connect(ui->optionedit,&OptionEdit::farPerspectiveDistanceChanged,
            ui->glview,&Viewer::setPerspectiveFar);
    connect(ui->optionedit,&OptionEdit::nearPerspectiveDistanceChanged,
            ui->glview,&Viewer::setPerspectiveNear);
    connect(ui->optionedit,&OptionEdit::resetObjectPosSignal,
            ui->glview,&Viewer::resetObjectPos);
    connect(ui->optionedit,&OptionEdit::shadermodeChanged,
            ui->glview,&Viewer::setShaderMode);

    connect(ui->staticlight,&StaticLightOptionEdit::ambientColorChanged,
            ui->glview,&Viewer::setStaticLightAmbient);
    connect(ui->staticlight,&StaticLightOptionEdit::diffuseColorChanged,
            ui->glview,&Viewer::setStaticLightDiffuse);
    connect(ui->staticlight,&StaticLightOptionEdit::specularColorChanged,
            ui->glview,&Viewer::setStaticLightSpecular);
    connect(ui->staticlight,&StaticLightOptionEdit::lightStateChanged,
            ui->glview,&Viewer::setStaticLightState);

    connect(ui->dynamiclight,&DynamicLightOptionEdit::ambientColorChanged,
            ui->glview,&Viewer::setDynamicLightAmbient);
    connect(ui->dynamiclight,&DynamicLightOptionEdit::diffuseColorChanged,
            ui->glview,&Viewer::setDynamicLightDiffuse);
    connect(ui->dynamiclight,&DynamicLightOptionEdit::specularColorChanged,
            ui->glview,&Viewer::setDynamicLightSpecular);
    connect(ui->dynamiclight,&DynamicLightOptionEdit::lightStateChanged,
            ui->glview,&Viewer::setDynamicLightState);
    connect(ui->dynamiclight,&DynamicLightOptionEdit::lightRotateSpeedChanged,
            ui->glview,&Viewer::setDynamicLightRotateSpeed);
    connect(ui->dynamiclight,&DynamicLightOptionEdit::rotateAxisChanged,
            ui->glview,&Viewer::setDynamicLightRotateAxis);

}

MainWindow::~MainWindow()
{
    delete ui;
}


