#ifndef TRANSP_H
#define TRANSP_H

#include <QObject>
#include <QQueue>
#include "types.h"

class QTimer;
class Slip;
class QByteArray;
class Transp : public QObject {
    Q_OBJECT

public:
    Transp(Slip *slip);
    ~Transp();
    void sendPacket(const QByteArray &bytes);
    int getQueueCount();
    void clearQueue();

signals:
    void answerReceive(QByteArray &bytes);
    void transpError();
    void reSentInc();
    void serialNotOpened();

private:
    Slip *m_slip;
    QQueue<QByteArray> sendQueue;
    QTimer *timeout;
    int waitACK = 0;
    int repeatCount = 0;

    uint16_t crc16(const QByteArray &buf, int len);
    bool checkCrc16(QByteArray &buf);
    union_uint16 getid(const QByteArray &buf);

private slots:
    void slipPacketReceive(QByteArray &bytes);
    void timeoutHandler();
    void serialClosed();
};

#endif // TRANSP_H
