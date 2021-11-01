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

#include "settingsshadowsfinddialog.h"
#include "maincontrolwidget.h"
#include "managementwidget.h"
#include "catchdatadialog.h"
#include "qcustomplot/qcustomplot.h"
#include <QRandomGenerator>
#include "savelog.h"
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
const unsigned short NEWSHADFINDPAR   = 0x17;
const unsigned short REQUEST_DIAMETER = 0x20;

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
    void sendByteToMK(char dst, char dataByte, const QString &msg);
    void sendVectorToMK(char dst, QVector<double> dataV, const QString &msg);
    void chOrderSend(int);
    int countCheckedCH(void);
    void manualGetShotButton();
    void getPacketFromMCU(unsigned short n);
    void getButtonClicked(bool checked);
    void selectShot(int index);
    void on_clearButton();

    //Slip handlers
    void handlerTranspAnswerReceive(QByteArray &bytes);
    void handlerTranspError();
    void reSentInc();
    void handlerTimer();
    void handlerGettingDiameterTimer();

    //Запись в файл
    void writeToLogfileMeta(QString name);
    //отрисовка таблицы
    void fillTable(QCPGraphDataContainer &dataMap);
    void on_ShdowSet_triggered();

    //Изменение настроек расчетов
    void settingsChanged();

    //Запись лога
    void saveHistory(QString &dirname);
private:
    Ui::MainWindow *ui;
    SerialSettings *settings_ptr;
    QSerialPort *serial;
    Slip *m_slip;
    Transp *m_transp;
    QTimer *m_timer,*m_GettingDiameterTimer;

    QVector <double> fromBytes(QByteArray &bytes);
    //Интерфейс
    QVBoxLayout *layoutV;
    MainControlWidget *m_MainControlWidget;
    ManagementWidget *m_ManagementWidget;
    QTableWidget *m_table;
    Console *m_console;
    QTabWidget *m_tab;
    QCustomPlot* diameterPlot;
    ShotViewer *viewer;
    firFilter *filter;
    SettingsShadowsFindDialog *ShadowSettings;
    StatusBar *statusBar;
    SaveLog *log;

    QLabel *tableSizeLabel;
    QSpinBox  *tableSizeSpinbox;

    //Переменные
    QMap<int,QByteArray> shotsCH1,shotsCH2,shotsCH2In,shotsCH3,shotsCH4,shotsCH4In;
    QByteArray currentShot;
    int shotCountRecieved=0;                                        //Текущее количество отмеченных каналов и текущее количество принятых шотов
    int packetSize=100, countAvaibleDots=0,countWaitingDots=0;           //Размер рабиения (100 по умолчанию), количество доступных точек в плате, количество ожидаемых точек от платы
    int countRecievedDots=0, channelsOrder=0;                    //Количество полученных точек, последовательность каналов, отправляемая в плату
    int notYetFlag=0;                                                       //Флаг, означающий, что не все каналы запрошеы и получены (если отмечено более одного канала, а кнопку получить жмем 1 раз)
    QVector<double> shadowsCh1,shadowsCh2,shadowsCh1Plis,shadowsCh2Plis;
    QVector<double> diameter, diameterPlis;
    QVector<double> tempPLISextremums1{3735,3795,5135,5191},tempPLISextremums2{973,1035,3010,3078};

    int tableSize=100;
    bool m_online = false;

    //Работа с файлами
    QDir *dir;
    QString dirnameDefault = "log";
    QString filename;
    QFile *file;
    QByteArray endShotLine = QByteArray::fromRawData("\xFF\x00\xFF\x00", 4);
    QByteArray endChannelLine = QByteArray::fromRawData("\xFE\x00\xFE\x00", 4);
    union conversation_t{
        char ch[8];
        double d;
    };
    union charToShort_t{
        char ch[2];
        unsigned short sh;
    };
    charToShort_t charToShort;
    QVector<double> diametersFromMCU,diameterKeys;
    //Для тестов
    void constructorTest();
    QByteArray generateBytes(int count);
    void plotDiameter();
    QVector<double> xDiameter,yDiameter;
    int filled = 0,lastIndex=0;
};

#endif // MAINWINDOW_H
