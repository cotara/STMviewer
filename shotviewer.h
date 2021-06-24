#ifndef SHOTVIEWER_H
#define SHOTVIEWER_H
#include "qcustomplot/qcustomplot.h"
#include <QWidget>

namespace Ui {
    class ShotViewer;
}

class ShotViewer : public QWidget
{
    Q_OBJECT
public:
    enum viewerChannels{
        CH1 = 0x01,
        CH2 = 0x02,

        AllCH = CH1 | CH2
    };
    explicit ShotViewer(QWidget *parent = nullptr);
    void showGraphs(int);
    void clearGraphs(int);
    void autoScale();
    void addUserGraph(QByteArray &buf, int len, int ch);
    void addDots(QVector<QVector<double>> dots, int ch);
    void addLines(QVector<double> dots, int ch);
signals:

private slots:
    void titleDoubleClick1(QMouseEvent *event);
    void titleDoubleClick2(QMouseEvent *event);
    void legendDoubleClick(QCPLegend* legend, QCPAbstractLegendItem* item);
    void selectionChanged1();
    void selectionChanged2();
    void mousePress1(QMouseEvent*);
    void mousePress2(QMouseEvent*);
    void mouseWheel1();
    void mouseWheel2();
    void graphClicked1(QCPAbstractPlottable *plottable, int dataIndex,QMouseEvent*);
    void graphClicked2(QCPAbstractPlottable *plottable, int dataIndex,QMouseEvent*);
    void graphDoubleClicked1();
    void graphDoubleClicked2();
    void showPointToolTip(QMouseEvent*);


private:
        QVBoxLayout *layoutV;
        QCustomPlot *customPlot1, *customPlot2;
        QPoint currentMousePosition;
        QCPItemText *textLabel1,*textLabel2,*textLabel3,*textLabel4,*textLabel5,*textLabel6;
        QCPItemTracer *tracer1,*tracer2;
};
#endif // SHOTVIEWER_H
