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
const unsigned short CH1              = 0x01;
const unsigned short CH2              = 0x02;
const unsigned short CH3              = 0x03;
const unsigned short CH4              = 0x04;

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
    void incCountCh(bool);
    void manualGetShotButton();
    void getPacketFromMCU(short n, const unsigned short ch);

    void consoleEnabledCheked(bool);
    void autoRangeGraphClicked();

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
    void addUserGraph(QByteArray &buf, int len, int ch);

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
    QVBoxLayout *controlLayout,*transmitLayout, *appSettingsLayout, *logLayout, *historyLayout;
    QLabel *packetSizeLabel;
    QSpinBox *packetSizeSpinbox;
    QPushButton *getButton, *autoRangeGraph, *clearButton;
    QGroupBox *transmitGroup, *appSettingsGroup, *logGroup, *historyGrouop;
    QCheckBox *autoGetCheckBox, *autoSaveShotCheckBox, *consoleEnable;
    QCheckBox *ch1CheckBox, *ch2CheckBox, *ch3CheckBox, *ch4CheckBox;
    QComboBox *shotsComboBox;
    QSpacerItem *m_spacer;

    QMap<int,QByteArray> shotsCH1,shotsCH2,shotsCH3,shotsCH4;
    QByteArray nowShotCH1,nowShotCH2,nowShotCH3,nowShotCH4;
    int chCountChecked=0,shotCountRecieved=0; //Текущее количество отмеченных каналов и текущее количество принятых шотов
    short packetSize=100;
    short countAvaibleDots=0,countAvaibleDotsCH1=0,countAvaibleDotsCH2=0,countAvaibleDotsCH3=0,countAvaibleDotsCH4=0;
    unsigned short countRecievedDots=0;
    QString dirname = "log";
    QString filename;
    QFile file;
    bool m_online = false;
};

#endif // MAINWINDOW_H
