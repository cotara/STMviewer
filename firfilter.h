#ifndef FIRFILTER_H
#define FIRFILTER_H
#include "qfile.h"
#include <QWidget>

class firFilter: public QWidget
{
    Q_OBJECT

public:
    firFilter(QVector<double>& s);
    ~firFilter();
    QByteArray toFilter(QByteArray &in,int len);
    QByteArray toButterFilter(QByteArray &in,int len);
    QVector <QVector<double>> extrFind(QByteArray &in,int len);
    QVector <QVector<unsigned int>> extrFind2(QByteArray &in,int len);
    QVector<double> shadowFind(QVector<double> dots);
    QVector<double> shadowFind(QVector<unsigned int> dots);
    QVector<double> diameterFind(QVector<double> shadowsCh1,QVector<double> shadowsCh2);
    void updateSettings(QVector<double>& s);
    //double freqCalc(QList<double> dots,int len);
    QVector <double> medianFilter(QVector<double> data, int window, int average);

private:
    QFile *file;
    double koefs[201];
    int offset=0;
    //shadowsFind constants:
    double p1=2.51087470;
    double res=4;
    //Параметры читаются из файла

    double la=0;
    double Nx=0;
    double Ny=0;
    double Hx=0;
    double Hy=0;
    double Cx=0;
    double Cy=0;
    //Все в микронах
//    double la = 905.0;
//    double L = 207400;
//    double p1=2.51087470;
//    double p2=0.83484861;
//    double p3=0.45007122;
//    double res=4;

//    double Nx=5320;
//    double Ny=5320;
//    double Hx=207.4;
//    double Hy=207.4;
//    double Cx=73.4;
//    double Cy=73.4;
    const double b[5]={0.00002673490400527396,0.0001069396160210958,0.0001604094240316438,0.0001069396160210958,0.00002673490400527396};
    const double a[5]={1,-3.606129410855792,4.894109789457051,-2.961577327764605,0.674024707627429};
    //const double b[5]={449,1794,2691,1794,449};
    //const double a[5]={16777216,-60500812,82109537,-49687023,11308258};
};

#endif // FIRFILTER_H
