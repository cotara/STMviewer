#include "slip.h"
#include "console.h"
#include <QSerialPort>
#include <QDebug>

static const char END       = 0xC0;    /* indicates end of packet */
static const char ESC       = 0xDB;    /* indicates byte stuffing */
static const char ESC_END   = 0xDC;    /* ESC ESC_END means END data byte */
static const char ESC_ESC   = 0xDD;    /* ESC ESC_ESC means ESC data byte */

enum {
    STATE_OK ,
    STATE_ESC
};

Slip::Slip(QSerialPort *serial, Console *console) : serialPort(serial), slipConsole(console) {
    connect(serialPort, &QSerialPort::readyRead, [this]() {
        //qDebug() << serialPort->bytesAvailable();
        QByteArray bytes = serialPort->readAll();
        for (char i : bytes) {
            new_rx_byte(i);
        }
    });
    connect(serialPort,&QSerialPort::errorOccurred,this,&Slip::slipSerialError);
}

void Slip::sendPacket(const QByteArray &bytes) {
    if (!serialPort->isOpen()) {
        emit serialPortClosed();
        qDebug() << "Serial port not opened";
        return;
    }
    if(send_byte(END))       return;

    for (auto i : bytes) {
        if (i == END) {
            if(send_byte(ESC)) return;
            i = ESC_END;
        }
        if (i == ESC) {
            if(send_byte(ESC)) return;
            i = ESC_ESC;
        }
        if(send_byte(i)) return;
    }
    if(send_byte(END)) return;

    QByteArray temp;
    temp.append("(endPacket)\n");
    slipConsole->putData(temp);

}

bool Slip::send_byte(const char &byte) {
    if (serialPort->isOpen())
        serialPort->write(&byte, 1);


    QSerialPort::SerialPortError err = serialPort->error();
    if(err){
        qDebug() << "error: serial Port Error";
        emit serialPortError();
        return true;
    }
    QByteArray temp;
    temp.append(byte);
    slipConsole->putData(temp.toHex() + " ");
    return false;
}

void Slip::new_rx_byte(char byte) {

    switch(byte) {
    case END:
        if (rx_buffer.size()) {
            slipConsole->putData("<-");
            if(rx_buffer.size()<=20){
                slipConsole->putData(rx_buffer.toHex());
            }
            else{
                QByteArray startPart = rx_buffer.left(20);
                slipConsole->putData(startPart.toHex() + "...");
            }
            emit packetReceive(rx_buffer);
            rx_buffer.clear();
        }
        state = STATE_OK;
        break;
    case ESC:
        state = STATE_ESC;
        break;
    case ESC_END:
        if (state == STATE_ESC) {
            rx_buffer.append(END);
            state = STATE_OK;
            break;
        }
    case ESC_ESC:
        if (state == STATE_ESC) {
            rx_buffer.append(ESC);
            state = STATE_OK;
            break;
        }
    default:
        rx_buffer.append(byte);
        state = STATE_OK;
    }
}

void Slip::slipSerialError(){
    QSerialPort::SerialPortError err = serialPort->error();
    if(err){
       emit serialPortError();
       serialPort->clearError();
    }
}
