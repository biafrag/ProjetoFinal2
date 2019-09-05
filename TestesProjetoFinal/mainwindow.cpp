#include "mainwindow.h"
#include "ui_mainwindow.h"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_EQU3button_clicked()
{
    ui->openGLWidget->setMode(MeshTypes::EQU3);
}

void MainWindow::on_EST1button_clicked()
{
    ui->openGLWidget->setMode(MeshTypes::EST1);
}


void MainWindow::on_TUBButton_clicked()
{
    ui->openGLWidget->setMode(MeshTypes::TUB);
}


void MainWindow::on_CIVbutton_clicked()
{
    ui->openGLWidget->setMode(MeshTypes::CIV);
}


void MainWindow::on_ESFERAbutton_clicked()
{
    ui->openGLWidget->setMode(MeshTypes::ESFERA);
}

void MainWindow::on_CUBObutton_clicked()
{
    ui->openGLWidget->setMode(MeshTypes::CUBO);
}

void MainWindow::on_DODECAEDRObutton_clicked()
{
    ui->openGLWidget->setMode(MeshTypes::DODECAEDRO);
}
