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
        CH3 = 0x04,
        CH4 = 0x08,
        CH5 = 0x10,
        CH6 = 0x20,
        AllCH = CH1 | CH2 |CH3 |CH4 |CH5 |CH6
    };
    explicit ShotViewer(QWidget *parent = nullptr);
    void showGraphs(int);
    void clearGraphs(int);
    void autoScale();
    void addUserGraph(QByteArray &buf, int len, int ch);
    void addDots(QVector<QVector<double>> dots, int ch);
signals:

private slots:
    void titleDoubleClick1(QMouseEvent *event);
    void titleDoubleClick2(QMouseEvent *event);
    void legendDoubleClick(QCPLegend* legend, QCPAbstractLegendItem* item);
    void selectionChanged1();
    void selectionChanged2();
    void mousePress1();
    void mousePress2();
    void mouseWheel1();
    void mouseWheel2();
    void graphClicked1(QCPAbstractPlottable *plottable, int dataIndex);
    void graphClicked2(QCPAbstractPlottable *plottable, int dataIndex);
    void graphDoubleClicked1();
    void graphDoubleClicked2();
    void showPointToolTip(QMouseEvent*);

private:
        QVBoxLayout *layoutV;
        QCustomPlot *customPlot1, *customPlot2;
        QPointer<QCPGraph> mGraph1,mGraph2,mGraph3,mGraph4,mGraph5,mGraph6;
        QPoint currentMousePosition;
        QCPItemText *textLabel1,*textLabel2,*textLabel3,*textLabel4,*textLabel5,*textLabel6;
};
#endif // SHOTVIEWER_H
