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

    void on_CUBObutton_clicked();

    void on_DODECAEDRObutton_clicked();

    void on_BRINCOSbutton_clicked();

    void on_VENTILADORbutton_clicked();

    void on_LATAButton_clicked();

    void on_ROBObutton_clicked();

    void on_verticalSlider_actionTriggered(int action);

    void on_verticalSlider_valueChanged(int value);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H