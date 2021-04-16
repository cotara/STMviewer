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

    connect(serial, &QSerialPort::readyRead, this, &MainWindow::readData);
    ui->disconnect->setEnabled(false);
    ui->pushButton->setText("Start");
    ui->pushButton->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_settings_triggered()
{
    settings_ptr->show();
}

void MainWindow::on_connect_triggered()
{
    //Настраиваем компорт
    serial->setPortName(settings_ptr->getName());
    serial->setBaudRate(settings_ptr->getBoudeRate());
    serial->setDataBits(settings_ptr->getDataBits());
    serial->setParity(settings_ptr->getParity());
    serial->setStopBits(settings_ptr->getStopBits());
    if (serial->open(QIODevice::ReadWrite)){
        ui->statusBar->showMessage("Подключено к " + settings_ptr->getName());
        ui->connect->setEnabled(false);
        ui->settings->setEnabled(false);
        ui->disconnect->setEnabled(true);
        ui->pushButton->setEnabled(true);

    }
    else{
         ui->statusBar->showMessage("Невозможно подключиться COM-порту");
    }
}

void MainWindow::on_disconnect_triggered()
{
    if(serial->isOpen()){
        serial->close();
        ui->connect->setEnabled(true);
        ui->settings->setEnabled(true);
        ui->disconnect->setEnabled(false);
        ui->pushButton->setEnabled(false);
        ui->statusBar->showMessage("Отключено от " + settings_ptr->getName());
    }
    else {
        ui->statusBar->showMessage("Невозможно отключиться от COM-порта");
    }
}

//Прием данных из COM-порта
void MainWindow::readData()
{
    const QByteArray data = serial->readAll();
    static QByteArray stringa;

    for (char i : data){
        ui->label->setNum(i);
        }

}

void MainWindow::on_pushButton_clicked()
{
    if(!butState){
        QString message = "START";
                message+= '\n';
        serial->write(message.toUtf8().data());
        ui->pushButton->setText("Stop");
        butState=1;
    }
    else{
        QString message = "STOP";
                message+= '\n';
        serial->write(message.toUtf8().data());
        ui->pushButton->setText("Start");
        butState=0;
    }
}
