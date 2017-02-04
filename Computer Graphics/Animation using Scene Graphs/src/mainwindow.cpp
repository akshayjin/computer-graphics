#include <QtWidgets>
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "myglwidget.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->myGLWidget, SIGNAL(xTranslationChanged(int)), ui->xRotSlider, SLOT(setValue(int)));
    connect(ui->myGLWidget, SIGNAL(yTranslationChanged(int)), ui->yRotSlider, SLOT(setValue(int)));
    connect(ui->myGLWidget, SIGNAL(zTranslationChanged(int)), ui->zRotSlider, SLOT(setValue(int)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape)
        close();
    else
        QWidget::keyPressEvent(e);
}

void MainWindow::on_zRotSlider_valueChanged(int value)
{
    ui->myGLWidget->setZTranslation(value);
}

void MainWindow::on_yRotSlider_valueChanged(int value)
{
    ui->myGLWidget->setYTranslation(value);
}

void MainWindow::on_xRotSlider_valueChanged(int value)
{
    ui->myGLWidget->setXTranslation(value);
}

void MainWindow::init_widget(char *argv[])
{
    // Load .PLY files from command line arguments

    ui->myGLWidget->model.model3D_1.setCenter(.5,0,.5);
    ui->myGLWidget->model.model3D_1.texture_fun = 0;
    ui->myGLWidget->model.model3D_1.Load(argv[1]);
    ui->myGLWidget->model.model3D_2.setCenter(.5,0,.5);
    ui->myGLWidget->model.model3D_2.texture_fun = 0;
    ui->myGLWidget->model.model3D_2.Load(argv[3]);
}

void MainWindow::on_zoom_valueChanged(int value)
{
    ui->myGLWidget->setZoom(value);
}

void MainWindow::on_light0_clicked(bool checked)
{
    ui->myGLWidget->toggleLight0(checked);
}

void MainWindow::on_light1_clicked(bool checked)
{
    ui->myGLWidget->toggleLight1(checked);
}

void MainWindow::on_light2_clicked(bool checked)
{
    ui->myGLWidget->toggleLight2(checked);
}

void MainWindow::on_light3_clicked(bool checked)
{
    ui->myGLWidget->toggleLight3(checked);
}

void MainWindow::on_boundingBox_clicked(bool checked)
{
    ui->myGLWidget->toggleBB(checked);
}

void MainWindow::on_texture1_clicked(bool checked)
{
    ui->myGLWidget->toggleTex1(checked);
}

void MainWindow::on_texture2_clicked(bool checked)
{
    ui->myGLWidget->toggleTex2(checked);
}

void MainWindow::on_changeTex1_released()
{
    ui->myGLWidget->changeTex1();
}

void MainWindow::on_changeTex2_released()
{
    ui->myGLWidget->changeTex2();
}

void MainWindow::on_robotVel_valueChanged(int value)
{
    ui->myGLWidget->setRobotSpeed(value);
}

void MainWindow::on_droneVel_valueChanged(int value)
{
    ui->myGLWidget->setDroneSpeed(value);
}

void MainWindow::on_droneRotVal_valueChanged(int value)
{
    ui->myGLWidget->setDroneRotSpeed(value);
}

void MainWindow::on_toggleCamera_released()
{
     ui->myGLWidget->toggleCamera();
}
