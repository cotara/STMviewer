#ifndef FIRFILTER_H
#define FIRFILTER_H
#include "qfile.h"

class firFilter
{
public:
    firFilter();
    ~firFilter();
    QByteArray toFilter(QByteArray &in,int len);
    QVector <QVector<double>> maximumFind(QByteArray &in,int len,int scale);
    QVector <QVector<double>> extrFind(QByteArray &in,int len);
    QVector<double> shadowFind(QVector<double> dots);
    QVector<double> diameterFind(QVector <QVector<double>> shadowsCh1, QVector <QVector<double>> shadowsCh2);
    //double freqCalc(QList<double> dots,int len);

private:
    QFile *file;
    double koefs[201];
    int offset=0;
};

#endif // FIRFILTER_H
