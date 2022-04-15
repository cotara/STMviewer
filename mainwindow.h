#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QInputDialog>
#include "transport\serialsettings.h"

#include "console.h"
#include <QTimer>
#include <QVBoxLayout>
#include "transport\slip.h"
#include "console.h"
#include "QLabel"
#include "QSpinBox"
#include "QCheckBox"
#include "QComboBox"
#include "QGroupBox"
#include "shotviewer.h"
#include "firfilter.h"

#include "shadowSettings\settingsshadowsfinddialog.h"
#include "controlPanel\maincontrolwidget.h"
#include "managementPanel\managementwidget.h"
#include "shadowSettings\catchdatadialog.h"
#include "qcustomplot/qcustomplot.h"
#include <QRandomGenerator>
#include "savelog.h"
// answer status
const unsigned short FAIL             = 0x0000;
const unsigned short OK               = 0x0101;
const unsigned short DATA_READY       = 0xFFF1;
const unsigned short NO_DATA_READY    = 0xFFFF;

// commands
const char ASK_MCU          = 0x41;
const char REQUEST_STATUS   = 0x50;
const char REQUEST_POINTS   = 0x70;
const char WRITE_POINTS     = 0x74;
const char CH_ORDER         = 0x81;
const char CH1              = 0x01;
const char CH2              = 0x02;
const char CH3              = 0x04;
const char CH4              = 0x08;
const char LAZER1_SET       = 0x10;
const char LAZER2_SET       = 0x11;
const char LAZERS_SAVE      = 0x12;
const char LEFT_BORDER_SET  = 0x13;
const char RIGHT_BORDER_SET = 0x14;
const char COMP_CH1_SET     = 0x15;
const char COMP_CH2_SET     = 0x16;
const char NEWSHADFINDPAR   = 0x17;
const char REQUEST_DIAMETER = 0x72;

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
    void toDeveloperMode();
    void on_settings_triggered();
    void on_connect_triggered();
    void on_disconnect_triggered();
    void sendByteToMK(char dst, int dataByte, const QString &msg);
    void sendVectorToMK(char dst, QVector<double> dataV, const QString &msg);
    void chOrderSend(int);
    void getPacketFromMCU(int n);
    int countCheckedCH(void);
    void manualGetShotButton();
    void getButtonClicked(bool checked);
    void selectShot();
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


    void saveHistory(QString &dirname);     //Запись лога
    void on_action_triggered();             //Чтение лога

    void mouseWheel1();
    void mouseWheel2();
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
    int xWindowDiameter = 5000;
    int tableSize=100;
    bool m_online = false;
    QCPGraph *r1=nullptr,*r2=nullptr,*c1=nullptr,*c2=nullptr,*m1=nullptr,*m2=nullptr,*f1=nullptr,*f2=nullptr,*spec1=nullptr,*spec2=nullptr;
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


    //Построение диаметров
    QCustomPlot* diametersPlot, *spectrePlot;
    void plotDiameter();
    void realTimeDiameter();
    void collectDiameter();
    void furie( QVector<double> *in,QVector<double> *spectr,QVector<double> *out, double  cutOfFreq);

    void clearDiameterVectors();
    void addDataToGraph();
    QVector<double> xDiameter,yr1,yr2,yc1,yc2,ym1,ym2,yf1,yf2;//То, что выводится на график
    QVector<double> xFurie,ySpectr1,ySpectr2,yFurieFiltered1,yFurieFiltered2;//Фурье
    QVector<double> shadowsFromMCU,r1FromMCU,r2FromMCU,c1FromMCU,c2FromMCU,m1FromMCU,m2FromMCU;//То, что приходит с MCU
    int filled = 0,lastIndex=0;
    int m_windowSize,m_average,m_limit, m_furieLimit = 20;
    bool diameterMode=false;
};

#endif // MAINWINDOW_H
