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

private:
    Ui::MainWindow *ui;
    SerialSettings *settings_ptr;
    QSerialPort *serial;
};

#endif // MAINWINDOW_H
