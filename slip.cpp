#include "slip.h"
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

Slip::Slip(QSerialPort *serial) : serialPort(serial) {
    connect(serialPort, &QSerialPort::readyRead, [this]() {
        QByteArray bytes = serialPort->readAll();
        for (char i : bytes) {
            new_rx_byte(i);
        }
    });
}

void Slip::sendPacket(const QByteArray &bytes) {
    if (!serialPort->isOpen()) {
        qDebug() << "serial port not open";
        return;
    }

    send_byte(END);
    for (auto i : bytes) {
        if (i == END) {
            send_byte(ESC);
            i = ESC_END;
        }
        if (i == ESC) {
            send_byte(ESC);
            i = ESC_ESC;
        }
        send_byte(i);
    }
    send_byte(END);
}

void Slip::send_byte(const char &byte) {
    serialPort->write(&byte, 1);
}

void Slip::new_rx_byte(char byte) {
    switch(byte) {
    case END:
        if (rx_buffer.size()) {
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
