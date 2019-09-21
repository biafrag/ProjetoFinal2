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

void MainWindow::on_BRINCOSbutton_clicked()
{
    ui->openGLWidget->setMode(MeshTypes::BRINCOS);
}

void MainWindow::on_VENTILADORbutton_clicked()
{
    ui->openGLWidget->setMode(MeshTypes::VENTILADOR);
}

void MainWindow::on_LATAButton_clicked()
{
    ui->openGLWidget->setMode(MeshTypes::LATA);
}

void MainWindow::on_ROBObutton_clicked()
{
     ui->openGLWidget->setMode(MeshTypes::ROBO);
}

void MainWindow::on_verticalSlider_actionTriggered(int action)
{
    switch (action) {
    case 0:
        ui->openGLWidget->setMaterial(MaterialTypes::COPPER);
        break;
    case 1:
        ui->openGLWidget->setMaterial(MaterialTypes::GOLD);
        break;
    case 2:
        ui->openGLWidget->setMaterial(MaterialTypes::POLISHEDSILVER);
        break;
    case 3:
        ui->openGLWidget->setMaterial(MaterialTypes::SILVER);
        break;
    default:
        break;
    }
}

void MainWindow::on_verticalSlider_valueChanged(int value)
{
    switch (value) {
    case 0:
        ui->openGLWidget->setMaterial(MaterialTypes::COPPER);
        break;
    case 1:
        ui->openGLWidget->setMaterial(MaterialTypes::GOLD);
        break;
    case 2:
        ui->openGLWidget->setMaterial(MaterialTypes::POLISHEDSILVER);
        break;
    case 3:
        ui->openGLWidget->setMaterial(MaterialTypes::SILVER);
        break;
    default:
        break;
    }
}

void MainWindow::on_checkBox_clicked(bool checked)
{
    if(checked)
    {
        ui->openGLWidget->setPBR(1);
    }
    else
    {
        ui->openGLWidget->setPBR(0);
    }
}
