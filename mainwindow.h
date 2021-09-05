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
#include "firfilter.h"
#include "centerviewer.h"

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
const unsigned short LAZER1_SET       = 0x10;
const unsigned short LAZER2_SET       = 0x11;
const unsigned short LAZERS_SAVE      = 0x12;


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
    void sendLazer1(int lazer1Par);
    void sendLazer2(int lazer2Par);
    void sendSaveEeprom();
    void setPacketSize(unsigned short n);
    void incCountCh(int);
    void manualGetShotButton();
    void getPacketFromMCU(unsigned short n);
    void autoGetCheckBoxChanged(int);
    void consoleEnabledCheked(bool);
    void autoSaveShotCheked(bool);
    void selectShot(int index);
    void on_clearButton();

    //Slip handlers
    void handlerTranspAnswerReceive(QByteArray &bytes);
    void handlerTranspError();
    void reSentInc();
    void handlerTimer();
    void sendChannelOrder();

    //Запись в файл
    void writeToLogfile(QString name);

    //отрисовка таблицы
    void fillTable(QCPGraphDataContainer &dataMap);
private:
    Ui::MainWindow *ui;
    SerialSettings *settings_ptr;

    void constructorTest();

    Console *m_console;
    QSerialPort *serial;
    Slip *m_slip;
    Transp *m_transp;
    QTimer *m_timer;
    StatusBar *statusBar;
    ShotViewer *viewer;
    firFilter *filter;
    centerViewer *m_centerViewer;
    QDir *dir;
    //Итерфейс
    QHBoxLayout *layoutH;
    QVBoxLayout *layoutV;
    QVBoxLayout *graphsLayout,*controlLayout,*lazerLayout,*transmitLayout,*resultLayout, *appSettingsLayout, *logLayout, *historyLayout;
    QHBoxLayout *lazersHorizontalLayout;
    QVBoxLayout *lazer1SettingLayout, *lazer2SettingLayout;
    QGroupBox *lazerGroup, *transmitGroup,*resultGroup, *appSettingsGroup, *logGroup, *historyGrouop;
    QCheckBox *autoGetCheckBox, *autoSaveShotCheckBox, *consoleEnable;
    QCheckBox *ch1CheckBox, *ch2CheckBox, *ch3CheckBox, *ch4CheckBox,*ch2InCheckBox, *ch4InCheckBox;
    QComboBox *shotsComboBox;
    QLabel *lazer1Label, *lazer2Label,*packetSizeLabel;
    QLabel *diametrLabel,*leftShadow1Label,*rightShadow1Label,*leftShadow2Label,*rightShadow2Label,*centerPositionLabel;
    QSpinBox *packetSizeSpinbox, *lazer1Spinbox, *lazer2Spinbox;
    QPushButton *getButton, *autoRangeGraph, *clearButton, *lazersSaveButton;
    QTableWidget *m_table;
    QLabel ch1ShadowsLabel;
    QLabel ch2ShadowsLabel;

    //Переменные
    QMap<int,QByteArray> shotsCH1,shotsCH2,shotsCH2In,shotsCH3,shotsCH4,shotsCH4In;
    QByteArray currentShot;
    int chCountChecked=0,shotCountRecieved=0,chCountRecieved=0;                               //Текущее количество отмеченных каналов и текущее количество принятых шотов
    short packetSize=100, countAvaibleDots=0,countWaitingDots=0;           //Размер рабиения (100 по умолчанию), количество доступных точек в плате, количество ожидаемых точек от платы
    int countRecievedDots=0, channelsOrder=0;                    //Количество полученных точек, последовательность каналов, отправляемая в плату
    int notYetFlag=0;                                                       //Флаг, означающий, что не все каналы запрошеы и получены (если отмечено более одного канала, а кнопку получить жмем 1 раз)
    QVector<QVector<double>> shadowsCh1,shadowsCh2;
    QVector<double> diameter;
    QSignalMapper *signalMapper;
    //Работа с файлами
    QString dirname = "log";
    QString filename;
    QFile *file1,*file2,*file3,*file4;
    QVector<double> tempPLISextremums1,tempPLISextremums2;
    bool m_online = false;
    QByteArray endShotLine = QByteArray::fromRawData("\xFF\x00\xFF\x00", 4);
};

#endif // MAINWINDOW_H
