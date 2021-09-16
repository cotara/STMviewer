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
#include "settingsshadowsfinddialog.h"
#include <QLCDNumber>

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
const unsigned short LEFT_BORDER_SET  = 0x13;
const unsigned short RIGHT_BORDER_SET = 0x14;
const unsigned short COMP_CH1_SET     = 0x15;
const unsigned short COMP_CH2_SET     = 0x16;

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
    void sendBorderLeft(int leftBorderVal);
    void sendBorderRight(int leftBorderVal);
    void sendCompCH1(int leftBorderVal);
    void sendCompCH2(int leftBorderVal);
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
    void writeToLogfileMeta(QString name);
    //отрисовка таблицы
    void fillTable(QCPGraphDataContainer &dataMap);
    void on_ShdowSet_triggered();

    //Изменение настроек расчетов
    void settingsChanged();

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
    SettingsShadowsFindDialog *ShadowSettings;
    QDir *dir;
    //Итерфейс
    QWidget *settingsWidget;
    QHBoxLayout *layoutH;
    QVBoxLayout *layoutV;
    QVBoxLayout *scrolLayout, *controlLayout,*transmitLayout, *resultLayout, *appSettingsLayout, *logLayout, *historyLayout, *errorsLayout;
    QHBoxLayout *lazerLayout,*borderLayout, *tableSizeLayout, *err1Layout, *err2Layout;
    QVBoxLayout *lazer1SettingLayout, *lazer2SettingLayout,*borderLeftLayout,*borderRightLayout,*compCH1Layout,*compCH2Layout;
    QGroupBox *lazerGroup, *transmitGroup, *borderGroup, *resultGroup, *appSettingsGroup, *logGroup, *historyGrouop, *errorsGroup;
    QScrollArea *scroll;
    QCheckBox *autoGetCheckBox, *autoSaveShotCheckBox, *consoleEnable, *tableEnable;
    QCheckBox *ch1CheckBox, *ch2CheckBox, *ch3CheckBox, *ch4CheckBox,*ch2InCheckBox, *ch4InCheckBox;
    QComboBox *shotsComboBox;
    QLabel *lazer1Label, *lazer2Label,*packetSizeLabel,*borderLeftLabel, *borderRightLabel,*compCH1Label,*compCH2Label;
    QLabel *diametrLabel,*diametrPlisLabel,*leftShadow1Label,*rightShadow1Label,*leftShadow2Label,*rightShadow2Label,*centerPositionLabel,*tableSizeLabel;
    QLabel *extr1Ch1,*extr2Ch1,*extr3Ch1,*extr4Ch1,*extr1Ch2,*extr2Ch2,*extr3Ch2,*extr4Ch2;
    QLabel *shad1Ch1,*shad2Ch1,*shad1Ch2,*shad2Ch2;
    QLabel *diam1,*diam2;


    QSpinBox *packetSizeSpinbox, *lazer1Spinbox, *lazer2Spinbox, *borderLeftSpinbox, *borderRightSpinbox,*compCH1Spinbox,*compCH2Spinbox,*tableSizeSpinbox;
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
    QVector<double> shadowsCh1,shadowsCh2,shadowsCh1Plis,shadowsCh2Plis;
    QVector<double> diameter, diameterPlis;
    QSignalMapper *signalMapper;
    int tableSize=100;
    //Работа с файлами
    QString dirname = "log";
    QString filename;
    QFile *file1,*file2,*file3,*file4;
    QVector<double> tempPLISextremums1,tempPLISextremums2;
    bool m_online = false;
    QByteArray endShotLine = QByteArray::fromRawData("\xFF\x00\xFF\x00", 4);
    QLCDNumber *err1,*err2,*err3,*err4,*err5,*err6,*err7,*err0;
};

#endif // MAINWINDOW_H
