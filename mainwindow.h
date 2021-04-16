#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "serialsettings.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();



private slots:
    void on_settings_triggered();

    void on_connect_triggered();

    void on_disconnect_triggered();

    void readData();

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    SerialSettings *settings_ptr;
    QSerialPort *serial;
    int butState=0;
};

#endif // MAINWINDOW_H
