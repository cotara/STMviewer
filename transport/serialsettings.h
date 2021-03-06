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
     void setName(QString ch){ name = ch;}
     void setBoudeRate(QSerialPort::BaudRate n){ boudeRate = n;}
     void setDataBits(QSerialPort::DataBits n){ dataBits=n;}
     void setParity(QSerialPort::Parity n){parity=n;}
     void setStopBits(QSerialPort::StopBits n){stopBits=n;}
     void setSlaveAdd(int n){slaveAddres = n;}
     QString getName(){return name;}
     QSerialPort::BaudRate getBoudeRate(){return boudeRate;}
     QSerialPort::DataBits getDataBits(){ return dataBits; }
     QSerialPort::Parity getParity(){return parity;}
     QSerialPort::StopBits getStopBits(){return stopBits;}
     int getSlaveAdd(){return slaveAddres;}
     void updateSettings();

private:
    QSerialPort::BaudRate boudeRate = QSerialPort::Baud115200;
    QSerialPort::DataBits dataBits = QSerialPort::Data8;
    QSerialPort::Parity parity = QSerialPort::NoParity;
    QSerialPort::StopBits stopBits = QSerialPort::OneStop;
    QString name = nullptr;
    int slaveAddres = 1;

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
};

#endif // SERIALSETTINGS_H
