#ifndef SLIP_H
#define SLIP_H

#include <QObject>

class QSerialPort;
class QByteArray;
class Slip : public QObject {
    Q_OBJECT

public:
    Slip(QSerialPort *serial);
    void sendPacket(const QByteArray &bytes);

signals:
    void packetReceive(QByteArray &bytes);

private:
    QSerialPort *serialPort;
    int state = 0;
    QByteArray rx_buffer;

    void send_byte(const char &byte);
    void new_rx_byte(char byte);
};

#endif // SLIP_H
