#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QInputDialog>
#include "serialsettings.h"
#include "qcustomplot/qcustomplot.h"
#include <QTimer>
#include <QVBoxLayout>


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
const char REQUEST_POINTS   = 0x70;


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

private slots:
    void on_settings_triggered();
    void on_connect_triggered();
    void on_disconnect_triggered();
    void manualGetShotButton();
    void selectShot(int index);

    //customPlots
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
    void handlerTimer();


private:
    Ui::MainWindow *ui;
    SerialSettings *settings_ptr;
    QCustomPlot *customPlot;
    QSerialPort *serial;
    Transp *m_transp;
    QTimer *m_timer;
    StatusBar *statusBar;

    QHBoxLayout *layout;
    QVBoxLayout *controlLayout;
    QPushButton *getButton;
    QGroupBox *controlGroup;
    QCheckBox * autoGetCheckBox, *autoSaveShotCheckBox;
    QComboBox *shotsComboBox;

    QList<QByteArray> shots;
    QString dirname = "log";
    QString filename;
    QFile file;
    bool m_online = false;

};

#endif // MAINWINDOW_H
