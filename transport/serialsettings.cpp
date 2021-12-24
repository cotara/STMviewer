#include "serialsettings.h"
#include "ui_serialsettings.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>
#include <iostream>
#include <QMessageBox>

SerialSettings::SerialSettings(QWidget *parent) : QDialog(parent), serialSettings(new Ui::SerialSettings)
{
    serialSettings->setupUi(this);
    setObjectName("serialsettings");   
    if(!fill_all()){
        QMessageBox::warning(this,"Внимание!","В системе нет ни одного COM-порта");
    }
    setWindowTitle("Настройки подключения");
    waitingD = new WaitingDialog(this);
}

SerialSettings::~SerialSettings()
{
    //delete serialSettings;
}

bool SerialSettings::fill_all()
{
    QStringList list;
    //Заполнение перечня доступных компортов
    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    if (ports.isEmpty())
        return false;
    for (QSerialPortInfo port : ports) {
        list << port.portName();
     }

    serialSettings->SerialSelect->addItems(list);

     for (int i=0; i<ports.count();i++){
        QSerialPortInfo p = ports.at(i);
        QString descr = p.description();
        QList<QString> list_temp=descr.split(' ');
        for (int k=0;k<list_temp.count();k++){
            if (list_temp.at(k).length() > 0){
                if (list_temp.at(k) == "CH340" || list_temp.at(k) == "ICPDAS"){
                    on_SerialSelect_currentIndexChanged(i);
                    serialSettings->SerialSelect->setCurrentText(ports.at(i).portName());
                    fill_serial_desctipton(i);
                }
            }
        }
     }

    //Заполнение настроек компорта
    serialSettings->speedBox->addItem(QStringLiteral("460800"), 460800);
    serialSettings->speedBox->addItem(QStringLiteral("921600"), 921600);
    serialSettings->speedBox->addItem(QStringLiteral("230400"), 230400);
    serialSettings->speedBox->addItem(QStringLiteral("115200"), QSerialPort::Baud115200);
    serialSettings->speedBox->addItem(QStringLiteral("9600"), QSerialPort::Baud9600);
    serialSettings->speedBox->addItem(QStringLiteral("19200"), QSerialPort::Baud19200);
    serialSettings->speedBox->addItem(QStringLiteral("38400"), QSerialPort::Baud38400);


    serialSettings->dataBitBox->addItem(QStringLiteral("5"), QSerialPort::Data5);
    serialSettings->dataBitBox->addItem(QStringLiteral("6"), QSerialPort::Data6);
    serialSettings->dataBitBox->addItem(QStringLiteral("7"), QSerialPort::Data7);
    serialSettings->dataBitBox->addItem(QStringLiteral("8"), QSerialPort::Data8);
    serialSettings->dataBitBox->setCurrentIndex(3);

    serialSettings->parityBox->addItem(tr("None"), QSerialPort::NoParity);
    serialSettings->parityBox->addItem(tr("Even"), QSerialPort::EvenParity);
    serialSettings->parityBox->addItem(tr("Odd"), QSerialPort::OddParity);
    serialSettings->parityBox->addItem(tr("Mark"), QSerialPort::MarkParity);
    serialSettings->parityBox->addItem(tr("Space"), QSerialPort::SpaceParity);

    serialSettings->stopBitBox->addItem(QStringLiteral("1"), QSerialPort::OneStop);
    serialSettings->stopBitBox->addItem(tr("1.5"), QSerialPort::OneAndHalfStop);
    serialSettings->stopBitBox->addItem(QStringLiteral("2"), QSerialPort::TwoStop);
    serialSettings->stopBitBox->setCurrentText(QVariant::fromValue(stopBits).toString());

    updateSettings();
    return true;

}

bool SerialSettings::fill_serial_desctipton(int i)
{
    if (i != -1) {
        QSerialPortInfo serial_info;
        QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
        int mun = ports.count();
        if (mun-1<i){
                QMessageBox::critical(this,"Внимание!","COM-порт отсутсвует");
                on_UpdateAvaiblePorts_clicked();
                return false;
        }
        serial_info=ports.takeAt(i);

        serialSettings->SerialDescription->setText(serial_info.description());
        serialSettings->SerialManufacturer->setText(serial_info.manufacturer());
        serialSettings->SerialSN->setText(serial_info.serialNumber());
        return true;
    }
    else
        return false;

}

void SerialSettings::clear_all_boxes()
{
    serialSettings->speedBox->clear();
    serialSettings->dataBitBox->clear();
    serialSettings->parityBox->clear();
    serialSettings->stopBitBox->clear();
    serialSettings->SerialSelect->clear();
}

void SerialSettings::on_SerialSelect_currentIndexChanged(int index){
    fill_serial_desctipton(index);
}

void SerialSettings::updateSettings()
{
    setBoudeRate(static_cast<QSerialPort::BaudRate>(serialSettings->speedBox->currentData().toInt()));
    setDataBits(static_cast<QSerialPort::DataBits>(serialSettings->dataBitBox->currentData().toInt()));
    setParity(static_cast<QSerialPort::Parity>(serialSettings->parityBox->currentData().toInt()));
    setStopBits(static_cast<QSerialPort::StopBits>(serialSettings->stopBitBox->currentData().toInt()));
    setName(serialSettings->SerialSelect->currentText());
    qDebug() << getBoudeRate() << " " << getDataBits() << " " << getParity() << " " <<getStopBits();
}


void SerialSettings::on_Ok_Cancel_box_clicked(QAbstractButton *button)
{
    if (serialSettings->Ok_Cancel_box->standardButton(button) == QDialogButtonBox::Ok)
        updateSettings();

    hide();
}



void SerialSettings::setName(QString ch){
    name = ch;
}

void SerialSettings::setBoudeRate(QSerialPort::BaudRate n){
    boudeRate = n;
}
void SerialSettings::setDataBits(QSerialPort::DataBits n){
    dataBits=n;
}
void SerialSettings::setParity(QSerialPort::Parity n){
    parity=n;
}
void SerialSettings::setStopBits(QSerialPort::StopBits n){
    stopBits=n;
}

QString SerialSettings::getName(){
    return name;
}
QSerialPort::BaudRate SerialSettings::getBoudeRate(){
    return boudeRate;
}
QSerialPort::DataBits SerialSettings::getDataBits(){
    return dataBits;
}
QSerialPort::Parity SerialSettings::getParity(){
    return parity;
}
QSerialPort::StopBits SerialSettings::getStopBits(){
    return stopBits;
}

void SerialSettings::on_UpdateAvaiblePorts_clicked()
{
    clear_all_boxes();
    fill_all();
}
//Переключение в девелопер мод
void SerialSettings::on_developerButton_clicked()
{
    QDialog dial;
    m_serial = new QSerialPort();
    m_serial->setBaudRate(static_cast<QSerialPort::BaudRate>(serialSettings->speedBox->currentData().toInt()));
    m_serial->setDataBits(dataBits);
    m_serial->setStopBits(stopBits);
    m_serial->setParity(parity);
    m_serial->setPortName(serialSettings->SerialSelect->currentText());
    if (m_serial->open(QIODevice::ReadWrite)){
        QByteArray command;
        command.append(serialSettings->devAddBox->value());
        command.append(char(0x06));
        command.append(char(0x00));
        command.append(char(0x02));
        command.append(char(0x00));
        command.append(char(0x01));

        int crc = 0xFFFF;
        for (int pos = 0; pos < 6; pos++) {
            crc ^= (int) command[pos] & 0xFF;

            for (int i = 8; i != 0; i--) {
                if ((crc & 0x0001) != 0) {
                    crc >>= 1;
                    crc ^= 0xA001;
                } else
                    crc >>= 1;
            }
        }
        command.append(char(crc>>8));
        command.append(char(crc&0xFF));

        if(m_serial->write(command)==8)
                waitingD->waitingStart(5000);
        else
            QMessageBox::critical(this, "Ошибка!","Отправка команды завершилась неудачей!",QMessageBox::Ok);
        m_serial->close();
    }
    else
        QMessageBox::critical(this, "Ошибка!","Невозможно открыть указанный COM-порт!",QMessageBox::Ok);


    delete m_serial;
    m_serial=nullptr;
}
