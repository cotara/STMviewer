#ifndef FIRFILTER_H
#define FIRFILTER_H
#include "qfile.h"

class firFilter
{
public:
    firFilter();
    ~firFilter();
    QByteArray toFilter(QByteArray &in,int len);
    QByteArray toButterFilter(QByteArray &in,int len);
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
    const double b[5]={0.00002673490400527396,0.0001069396160210958,0.0001604094240316438,0.0001069396160210958,0.00002673490400527396};
    const double a[5]={1,-3.606129410855792,4.894109789457051,-2.961577327764605,0.674024707627429};
    //const double b[5]={449,1794,2691,1794,449};
    //const double a[5]={16777216,-60500812,82109537,-49687023,11308258};
};

#endif // FIRFILTER_H
