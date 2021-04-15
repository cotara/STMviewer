#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "serialsettings.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    settings_ptr = new SerialSettings();
    serial = new QSerialPort();
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_settings_triggered()
{
    settings_ptr->show();
}
