#ifndef DIAMETERVIEWER_H
#define DIAMETERVIEWER_H

#include <QWidget>
#include <QVBoxLayout>
#include "qcustomplot/qcustomplot.h"

class DiameterViewer : public QWidget
{
    Q_OBJECT
public:
    explicit DiameterViewer(QWidget *parent = nullptr);

public slots:
    void addDataToGraph(const QVector<double> &d1, const QVector<double> &d2, const QVector<double> &c1, const QVector<double> &c2, const QVector<double> &m1, const QVector<double> &m2);
    void clearGraphs();
    void setDiamEnPlot(bool en){m_diamEnPlot = en;}
    void setOffsetsEnPlot(bool en){m_offsetsEnPlot = en;}
    void setFilteredEnPlot(bool en){m_filteredEnPlot = en;}
    void setFurieEnPlot(bool en){m_furieEnPlot = en;}
    void setDiamPlotMode(bool en){m_diamPlotMode = en; clearDiameterVectors();}
    void setWindow(int val){m_window = val;}
    void setFurieFreq(double val){m_furieLimit = val;}
    void autoScale();
signals:

private:
    QCustomPlot *diameterPlot,*spectrePlot;
    void plotDiameter();
    QCPGraph *r1=nullptr,*r2=nullptr,*c1=nullptr,*c2=nullptr,*m1=nullptr,*m2=nullptr,*f1=nullptr,*f2=nullptr,*spec1=nullptr,*spec2=nullptr;
    bool m_diamEnPlot=false, m_offsetsEnPlot=false, m_filteredEnPlot = false,m_furieEnPlot = false;
    bool m_diamPlotMode = false;
    int m_window=1000;
    double m_furieLimit=3;
    QVector<double> xDiameter,yr1,yr2,yc1,yc2,ym1,ym2,yf1,yf2;//То, что выводится на график
    QVector<double> xFurie,ySpectr1,ySpectr2,yFurieFiltered1,yFurieFiltered2;//Фурье
    int lastIndex=0, filled = 0;

private slots:
    void furie(QVector<double> *in, QVector<double> *spectr, QVector<double> *out, double  cutOfFreq);
    void realTimeDiameter();
    void clearDiameterVectors();
    void collectDiameter();
    void mouseWheel1();
    void mouseWheel2();
};

#endif // DIAMETERVIEWER_H
