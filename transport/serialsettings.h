#ifndef SERIALSETTINGS_H
#define SERIALSETTINGS_H

#include <QDialog>
#include <QSerialPort>
#include <QAbstractButton>
#include "waitingdialog.h"
#include <QMessageBox>
namespace Ui {
    class SerialSettings;
}

class SerialSettings : public QDialog
{
    Q_OBJECT
public:
    explicit SerialSettings(QWidget *parent = nullptr);
    ~SerialSettings();

     bool fill_all();
     void setName(QString ch);
     void setBoudeRate(QSerialPort::BaudRate n);
     void setDataBits(QSerialPort::DataBits n);
     void setParity(QSerialPort::Parity n);
     void setStopBits(QSerialPort::StopBits n);
     QString getName();
     QSerialPort::BaudRate getBoudeRate();
     QSerialPort::DataBits getDataBits();
     QSerialPort::Parity getParity();
     QSerialPort::StopBits getStopBits();
     void updateSettings();

private:
    QSerialPort::BaudRate boudeRate = QSerialPort::Baud115200;
    QSerialPort::DataBits dataBits = QSerialPort::Data8;
    QSerialPort::Parity parity = QSerialPort::NoParity;
    QSerialPort::StopBits stopBits = QSerialPort::OneStop;
    QString name = nullptr;

    Ui::SerialSettings *serialSettings;

    bool fill_serial_desctipton(int i);
    void clear_all_boxes();

    QSerialPort *m_serial;
    WaitingDialog *waitingD;

signals:

public slots:

private slots:
    void on_SerialSelect_currentIndexChanged(int index);
    void on_Ok_Cancel_box_clicked(QAbstractButton *button);
    void on_UpdateAvaiblePorts_clicked();
    void on_developerButton_clicked();
    void SerialError(QSerialPort::SerialPortError error);
};

#endif // SERIALSETTINGS_H
