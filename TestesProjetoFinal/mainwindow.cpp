#include "mainwindow.h"
#include "ui_mainwindow.h"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->groupBox_2->hide();
    ui->groupBox_3->hide();
    ui->label->hide();
    ui->label_2->hide();
    ui->sizeImperfectionsSlider->hide();
    ui->numerImperfectionsSlider->hide();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_ESFERAbutton_clicked()
{
    ui->openGLWidget->setMode(MeshTypes::ESFERA);
}

void MainWindow::on_BRINCOSbutton_clicked()
{
    ui->openGLWidget->setMode(MeshTypes::BRINCOS);
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
        ui->DirtyCheckBox->hide();
        ui->groupBox_2->show();
    }
    else
    {
        ui->openGLWidget->setPBR(0);
//        ui->verticalSlider->show();
//        ui->COPPERLabel->show();
//        ui->GOLDLabel->show();
//        ui->POLSILVERLabel->show();
//        ui->SILVERLabel->show();
        ui->DirtyCheckBox->show();
        ui->groupBox_2->hide();
    }
}

void MainWindow::on_GoldRadioButton_clicked()
{
    ui->openGLWidget->setOption(Options::GOLD);
}

void MainWindow::on_radioButton_clicked()
{
        ui->openGLWidget->setMode(MeshTypes::TROFEU);
}

void MainWindow::on_NoNoiseRadioButton_clicked()
{
        ui->openGLWidget->setOption(Options::NONOISE);
}

void MainWindow::on_radioButton_3_clicked()
{
     ui->openGLWidget->setMode(MeshTypes::BULE);
}



void MainWindow::on_DirtyCheckBox_clicked(bool checked)
{
    if(checked)
    {
        ui->openGLWidget->setDirty(1);
    }
    else
    {
        ui->openGLWidget->setDirty(0);
    }
}

void MainWindow::on_Bump1radioButton_clicked()
{
    ui->openGLWidget->setBumpType(0);
}

void MainWindow::on_Bump2radioButton_clicked()
{
    ui->openGLWidget->setBumpType(1);
}

void MainWindow::on_Bump3radioButton_clicked()
{
    ui->openGLWidget->setBumpType(2);
}

void MainWindow::on_Bump4radioButton_clicked()
{
    ui->openGLWidget->setBumpType(3);
}

void MainWindow::on_BumpTesteradioButton_clicked()
{
    ui->openGLWidget->setBumpType(6);
}

void MainWindow::on_WithoutBumpradioButton_clicked()
{
    ui->openGLWidget->setBumpType(4);
}


void MainWindow::on_BumpWithColorradioButton_clicked()
{
    ui->openGLWidget->setBumpType(5);
}

void MainWindow::on_CloudRoughnessNoiseRadioButton_clicked()
{
    ui->openGLWidget->setOption(Options::CLOUDSROUGH);
}

void MainWindow::on_CloudsAlbedoRadioButton_clicked()
{
    ui->openGLWidget->setOption(Options::CLOUDSALB);
}

void MainWindow::on_PhongBumpRadioButton_toggled(bool checked)
{
    if(checked)
    {
        ui->openGLWidget->setOption(Options::NORMAL);
        ui->groupBox_3->show();
    }
    else
    {
        ui->groupBox_3->hide();
    }
}


void MainWindow::on_PBRgBumpRadioButton_toggled(bool checked)
{
    if(checked)
    {
        ui->openGLWidget->setOption(Options::NORMALPBR);
        ui->groupBox_3->show();
    }
    else
    {
        ui->groupBox_3->hide();
    }
}


void MainWindow::on_numerImperfectionsSlider_valueChanged(int value)
{
    ui->openGLWidget->setNumberImperfections(value);

}

void MainWindow::on_BumpTesteradioButton_toggled(bool checked)
{
    if(checked)
    {
        ui->label->show();
        ui->label_2->show();
        ui->sizeImperfectionsSlider->show();
        ui->numerImperfectionsSlider->show();
    }
    else
    {
        ui->label->hide();
        ui->label_2->hide();
        ui->sizeImperfectionsSlider->hide();
        ui->numerImperfectionsSlider->hide();
    }
}

void MainWindow::on_sizeImperfectionsSlider_valueChanged(int value)
{
    ui->openGLWidget->setSizeImperfections(value);
}
