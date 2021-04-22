#include "transp.h"
#include "slip.h"
#include <QTimer>
#include <QDebug>

#define TIMEOUT 500

Transp::Transp(Slip *slip) : m_slip(slip) {
    connect(m_slip, &Slip::packetReceive, this, Transp::slipPacketReceive);

    timeout = new QTimer;
    timeout->setInterval(TIMEOUT);

    connect(timeout, &QTimer::timeout, this, Transp::timeoutHandler);
}

Transp::~Transp() {
    delete timeout;
    delete m_slip;
}

void Transp::sendPacket(const QByteArray &bytes) {
    static union_uint16 nextPacketID = {0};

/****************************************************************/
/*                      Packet format:                          */
/*          +-----------+----------+------+----------+          */
/*          | packet id |  length  | data |  crc16   |          */
/*          | (2 byte)  | (2 byte) | (-)  | (2 byte) |          */
/*          +-----------+----------+------+----------+          */
/*                                                              */
/****************************************************************/

    QByteArray buff;
    union_uint16 length = {static_cast<uint16_t>(bytes.size() + 6)};

    buff.append(nextPacketID.bytes, 2);
    nextPacketID.in++;
    buff.append(length.bytes, 2);
    buff.append(bytes);
    uint16_t crc = crc16(buff, length.in - 2);
    buff.append(static_cast<char>(crc & 0xFF));
    buff.append(static_cast<char>(crc >> 8));
    sendQueue.enqueue(buff);

    if (!waitACK) {
        waitACK = 1;
        m_slip->sendPacket(buff);
        timeout->start();
    }
}

void Transp::slipPacketReceive(QByteArray &bytes) {
    if (waitACK) {
        if (checkCrc16(bytes)) {
            if (getid(bytes).in == getid(sendQueue.head()).in) {
                sendQueue.dequeue();
                timeout->stop();
                repeatCount = 0;
                bytes.remove(0, 4);
                bytes.remove(bytes.size() - 2, 2);
                emit answerReceive(bytes);
                if (!sendQueue.isEmpty()) {
                    waitACK = 1;
                    m_slip->sendPacket(sendQueue.head());
                    timeout->start();
                } else
                    waitACK = 0;
            } else {
                qDebug() << "error: wrong ACK ID";
            }
        }
        else {
            qDebug() << "error: wrong CRC";
        }
    }
}

void Transp::timeoutHandler() {
    if (repeatCount > 5) {
        timeout->stop();
        repeatCount = 0;
        waitACK = 0;
        qDebug() << "error: not ACK";
        emit transpError();
    } else {
        m_slip->sendPacket(sendQueue.head());
        qDebug() << "repeat send";
        repeatCount++;
    }
}

uint16_t Transp::crc16(const QByteArray &buf, int len) {
    int crc;
    crc = 0xFFFF;
    for (int pos = 0; pos < len; pos++) {
        crc ^= (int) buf[pos] & 0xFF;

        for (int i = 8; i != 0; i--) {
            if ((crc & 0x0001) != 0) {
                crc >>= 1;
                crc ^= 0xA001;
            } else
                crc >>= 1;
        }
    }
    return crc;
}

bool Transp::checkCrc16(QByteArray &buf) {
    int len = buf.length();
    if (len < 3)
        return false;
    uint16_t crc = crc16(buf, len - 2);
    return !memcmp(&crc, buf.data() + len - 2, 2);
}

union_uint16 Transp::getid(const QByteArray &buf) {
    union_uint16 id;
    memcpy(id.bytes, buf, 2);
    return id;
}
