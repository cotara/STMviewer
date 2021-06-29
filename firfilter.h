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
    //shadowsFind constants:
    const double la = 905.0/1000000000.0;
    const double L = 207.4/1000;
    const double p1=2.51087470;
    const double p2=0.83484861;
    const double p3=0.45007122;
    const double res=0.000004;

    const double Nx=5320;
    const double Ny=5320;
    const double Hx=207.4;
    const double Hy=207.4;
    const double Cx=73.4;
    const double Cy=73.4;
    const double res2 =  0.004;
};

#endif // FIRFILTER_H
