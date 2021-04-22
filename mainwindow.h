#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QInputDialog>
#include "serialsettings.h"
#include "qcustomplot/qcustomplot.h"
#include <QTimer>

// answer status
const char FAIL             = 0x00;
const char OK               = 0x01;

const char ON               = 0x02;
const char OFF              = 0x03;
const char DATA_READY       = 0x04;
const char NO_DATA_READY    = 0x05;

// commands
const char ASK_MCU          = 0x41;
const char REQUEST_STATUS   = 0x50;
const char REQUEST_TIME     = 0x71;
const char SET_TIME         = 0x72;
const char REQUEST_PROG     = 0x81;
const char SET_PROG         = 0x82;
const char MANUAL_CONTROL   = 0x91;
const char SET_MANUAL_MODE  = 0x93;
const char BALLAST_INIT     = 0xA0;

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
    void manualUpdate(bool);
    void daliUpdate(const uint32_t);

private slots:
    void on_settings_triggered();

    void on_connect_triggered();

    void on_disconnect_triggered();

    void readData();

    void on_pushButton_clicked();

    //customPlots
    void titleDoubleClick(QMouseEvent *event);
    void axisLabelDoubleClick(QCPAxis* axis, QCPAxis::SelectablePart part);
    void legendDoubleClick(QCPLegend* legend, QCPAbstractLegendItem* item);
    void selectionChanged();
    void mousePress();
    void mouseWheel();
    void addRandomGraph();
    void removeSelectedGraph();
    void removeAllGraphs();
    void contextMenuRequest(QPoint pos);
    void moveLegend();
    void graphClicked(QCPAbstractPlottable *plottable, int dataIndex);

    void addUserGraph(int* buf, int len);

    void handlerTranspAnswerReceive(QByteArray &bytes);
    void handlerTranspError();
    void handlerTimer();

private:
    Ui::MainWindow *ui;
    SerialSettings *settings_ptr;
    QCustomPlot *customPlot;
    QSerialPort *serial;
    int butState=0;
    int fromSTM[11000];
    int counter=0;

    bool m_online = false;
    Transp *m_transp;
    QTimer *m_timer;
    StatusBar *statusBar;

};

#endif // MAINWINDOW_H
