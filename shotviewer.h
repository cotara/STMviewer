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
    void addDots(QVector<QVector<unsigned int>> dots, int ch);
    void addLines(QVector<double> dots, int ch, int w);
    void addLines2(QVector<double> dots, int ch, int w);
    void replotGraphs(int state);
    void rescaleX(int xmin, int xmax);

signals:
    void graph_selected(QCPGraphDataContainer &dataMap);

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
    void graphClicked1(QCPAbstractPlottable *, int ,QMouseEvent*);
    void graphClicked2(QCPAbstractPlottable *, int, QMouseEvent*);
    void graphDoubleClicked1();
    void graphDoubleClicked2();
    void showPointToolTip(QMouseEvent*);
    void graphRangeChanged1();
    void graphRangeChanged2();

private:
        QVBoxLayout *layoutV;
        QCustomPlot *customPlot1, *customPlot2;
        QPoint currentMousePosition;
        QCPItemText *textLabel1,*textLabel2;
        QCPItemTracer *tracer1=nullptr,*tracer2=nullptr;
        int signalSize = 10800;
};
#endif // SHOTVIEWER_H
