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

void MainWindow::on_PLATAFORMAbutton_clicked()
{
     ui->openGLWidget->setMode(MeshTypes::PLATAFORMA);
}


void MainWindow::on_EQU1button_clicked()
{
    ui->openGLWidget->setMode(MeshTypes::EQU1);
}

void MainWindow::on_EQU2button_clicked()
{
     ui->openGLWidget->setMode(MeshTypes::EQU2);
}

void MainWindow::on_TUBButton_clicked()
{
    ui->openGLWidget->setMode(MeshTypes::TUB);
}
