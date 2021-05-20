#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QInputDialog>
#include "serialsettings.h"

#include "console.h"
#include <QTimer>
#include <QVBoxLayout>
#include "slip.h"
#include "console.h"
#include "QLabel"
#include "QSpinBox"
#include "QCheckBox"
#include "QComboBox"
#include "QGroupBox"
#include "shotviewer.h"

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
const unsigned short CH_ORDER         = 0x81;
const unsigned short CH1              = 0x01;
const unsigned short CH2              = 0x02;
const unsigned short CH3              = 0x04;
const unsigned short CH4              = 0x08;

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
    void getPacketFromMCU(short n);
    void autoGetCheckBoxChanged(int);
    void consoleEnabledCheked(bool);

    void selectShot(int index);
    void on_clearButton();

    //Slip handlers
    void handlerTranspAnswerReceive(QByteArray &bytes);
    void handlerTranspError();
    void reSentInc();
    void handlerTimer();

    void sendChannelOrder();
private:
    Ui::MainWindow *ui;
    SerialSettings *settings_ptr;

    Console *m_console;
    QSerialPort *serial;
    Slip *m_slip;
    Transp *m_transp;
    QTimer *m_timer;
    StatusBar *statusBar;
    ShotViewer *viewer;
    //Итерфейс
    QHBoxLayout *layoutH;
    QVBoxLayout *layoutV;
    QVBoxLayout *graphsLayout, *controlLayout,*transmitLayout,*signalProcessingLayout, *appSettingsLayout, *logLayout, *historyLayout;
    QGroupBox *transmitGroup, *signalProcessingGroup, *appSettingsGroup, *logGroup, *historyGrouop;
    QCheckBox *autoGetCheckBox, *autoSaveShotCheckBox, *consoleEnable;
    QCheckBox *ch1CheckBox, *ch2CheckBox, *ch3CheckBox, *ch4CheckBox;
    QComboBox *shotsComboBox;
    QLabel *packetSizeLabel;
    QSlider *shiftCH1NF_Slider, *shiftCH2NF_Slider;
    QSpinBox *packetSizeSpinbox;
    QPushButton *getButton, *autoRangeGraph, *clearButton;

    //Переменные
    QMap<int,QByteArray> shotsCH1,shotsCH2,shotsCH3,shotsCH4;
    QByteArray currentShot;
    int chCountChecked=0,shotCountRecieved=0;                               //Текущее количество отмеченных каналов и текущее количество принятых шотов
    short packetSize=100, countAvaibleDots=0,countWaitingDots=0;           //Размер рабиения (100 по умолчанию), количество доступных точек в плате, количество ожидаемых точек от платы
    short countRecievedDots=0, channelsOrder=0;                    //Количество полученных точек, последовательность каналов, отправляемая в плату
    int notYetFlag=0;                                                       //Флаг, означающий, что не все каналы запрошеы и получены (если отмечено более одного канала, а кнопку получить жмем 1 раз)

    //Работа с файлами
    QString dirname = "log";
    QString filename;
    QFile file;
    bool m_online = false;
};

#endif // MAINWINDOW_H
