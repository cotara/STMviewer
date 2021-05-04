#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QInputDialog>
#include "serialsettings.h"
#include "qcustomplot/qcustomplot.h"
#include "console.h"
#include <QTimer>
#include <QVBoxLayout>
#include "slip.h"
#include "console.h"
// answer status
const unsigned short FAIL             = 0x0000;
const unsigned short OK               = 0x0101;
const unsigned short DATA_READY       = 0xFFF1;
const unsigned short NO_DATA_READY    = 0xFFFF;

// commands
const unsigned short ASK_MCU          = 0x41;
const unsigned short REQUEST_STATUS   = 0x50;
const unsigned short REQUEST_POINTS   = 0x70;
const unsigned short WRITE_POINTS     = 0x74;


class QSerialPort;
class QTimer;
class Transp;
class StatusBar;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


signals:
    void statusUpdate(bool);
    void dataReadyUpdate(int);
    void infoUpdate(int);
    void downloadUpdate(bool);

private slots:
    void on_settings_triggered();
    void on_connect_triggered();
    void on_disconnect_triggered();
    void setPacketSize(short n);
    void manualGetShotButton();
    void getPacketFromMCU(short n);
    void consoleEnabledCheked(bool);
    void selectShot(int index);
    void on_clearButton();

    //customPlot
    void titleDoubleClick(QMouseEvent *event);
    void axisLabelDoubleClick(QCPAxis* axis, QCPAxis::SelectablePart part);
    void legendDoubleClick(QCPLegend* legend, QCPAbstractLegendItem* item);
    void selectionChanged();
    void mousePress();
    void mouseWheel();
    void moveLegend();
    void graphClicked(QCPAbstractPlottable *plottable, int dataIndex);
    void addUserGraph(QByteArray &buf, int len);

    //Slip handlers
    void handlerTranspAnswerReceive(QByteArray &bytes);
    void handlerTranspError();
    void reSentInc();
    void handlerTimer();

private:
    Ui::MainWindow *ui;
    SerialSettings *settings_ptr;
    QCustomPlot *customPlot;
    Console *m_console;
    QSerialPort *serial;
    Slip *m_slip;
    Transp *m_transp;
    QTimer *m_timer;
    StatusBar *statusBar;

    QHBoxLayout *layoutH;
    QVBoxLayout *layoutV;
    QVBoxLayout *controlLayout;
    QSpinBox *packetSizeSpinbox;
    QPushButton *getButton;
    QPushButton *clearButton;
    QGroupBox *controlGroup;
    QCheckBox * autoGetCheckBox, *autoSaveShotCheckBox, *consoleEnable;
    QComboBox *shotsComboBox;
    QSpacerItem *m_spacer;

    QList<QByteArray> shots;
    QByteArray nowShot;
    short packetSize=100;
    short countAvaibleDots=0;
    short countRecievedDots=0;
    QString dirname = "log";
    QString filename;
    QFile file;
    bool m_online = false;
};

#endif // MAINWINDOW_H
