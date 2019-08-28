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

    void on_PLATAFORMAbutton_clicked();


    void on_EQU1button_clicked();

    void on_EQU2button_clicked();

    void on_TUBButton_clicked();

    void on_EQU3button_clicked();

    void on_EST1button_clicked();

    void on_CIVbutton_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
