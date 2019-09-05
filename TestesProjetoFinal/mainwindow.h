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

    void on_TUBButton_clicked();

    void on_EQU3button_clicked();

    void on_EST1button_clicked();

    void on_CIVbutton_clicked();

    void on_ESFERAbutton_clicked();

    void on_CUBObutton_clicked();

    void on_DODECAEDRObutton_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
