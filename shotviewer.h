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
    enum viewerState{
        CH1_CH2,
        CH1_Only,
        CH2_Only,
        CH3_CH4,
        CH3_Only,
        CH4_Only,
        Both
    };
    explicit ShotViewer(QWidget *parent = nullptr);
    void showGraphs(viewerState);
    void clearGraphs(viewerState);
    void autoScale();
    void addUserGraph(QByteArray &buf, int len, int ch);

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
        QPointer<QCPGraph> mGraph1,mGraph2,mGraph3,mGraph4;
        QPoint currentMousePosition;
};
#endif // SHOTVIEWER_H
