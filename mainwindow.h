#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QInputDialog>
#include "serialsettings.h"
#include "qcustomplot/qcustomplot.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();



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

private:
    Ui::MainWindow *ui;
    SerialSettings *settings_ptr;
    QCustomPlot *customPlot;
    QSerialPort *serial;
    int butState=0;
    int fromSTM[10000];
    int counter=0;

};

#endif // MAINWINDOW_H
