#ifndef SLIP_H
#define SLIP_H

#include <QObject>

class QSerialPort;
class QByteArray;
class Console;
class Slip : public QObject {
    Q_OBJECT

public:
    Slip(QSerialPort *serial, Console *console);
    void sendPacket(const QByteArray &bytes);

signals:
    void packetReceive(QByteArray &bytes);
    void serialPortClosed();
    void serialPortError();
private:
    QSerialPort *serialPort;
    Console *slipConsole;
    int state = 0;
    QByteArray rx_buffer;


    bool send_byte(const char &byte);
    void new_rx_byte(char byte);
    void slipSerialError();
};

#endif // SLIP_H
