#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    void on_ESFERAbutton_clicked();

    void on_BRINCOSbutton_clicked();

    void on_ROBObutton_clicked();



    void on_checkBox_clicked(bool checked);

    void on_GoldRadioButton_clicked();

    void on_CloudRoughnessNoiseRadioButton_clicked();

    void on_radioButton_clicked();

    void on_NoNoiseRadioButton_clicked();
    void on_CloudsAlbedoRadioButton_clicked();

    void on_radioButton_3_clicked();



    void on_DirtyCheckBox_clicked(bool checked);

    void on_Bump1radioButton_clicked();

    void on_Bump2radioButton_clicked();

    void on_Bump3radioButton_clicked();

    void on_Bump4radioButton_clicked();

    void on_WithoutBumpradioButton_clicked();

    void on_BumpWithColorradioButton_clicked();

    void on_PhongBumpRadioButton_toggled(bool checked);


    void on_PBRgBumpRadioButton_toggled(bool checked);


    void on_BumpTesteradioButton_clicked();


    void on_BumpTesteradioButton_toggled(bool checked);

    void on_sizeImperfectionsSlider_valueChanged(int value);

    void on_BumpMarbleradioButton_clicked();

    void on_MarbleCheckBox_clicked(bool checked);

    void on_Bump4radioButton_toggled(bool checked);

    void on_DirtySlider_valueChanged(int value);

    void on_actionSave_image_triggered();

    void on_BumpMarbleradioButton_toggled(bool checked);

    void on_BumpWithColorradioButton_toggled(bool checked);

    void on_actionImport_Mesh_triggered();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
