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
    QVector <double> medianFilterX(QVector<double> data, int window, int average, int limit);
    double median_filter_x(double datum, int window);
    QVector <double> medianFilterY(QVector<double> data, int window, int average, int limit);
    double median_filter_y(double datum, int window);
    void setResolution(double resolution);
private:
    QFile *file;
    double koefs[201];

    int offset=0;
    //Константы для поиска тени по экстремумам:
    const double p1=2.51087470;
    const double la=0.905;

    double res=4;
    //Геометрические параметры, мкм
    double Nx=5320;
    double Ny=5320;
    double Hx=207.4;
    double Hy=207.4;
    double Cx=73.4;
    double Cy=73.4;

    const double b[5]={0.00002673490400527396,0.0001069396160210958,0.0001604094240316438,0.0001069396160210958,0.00002673490400527396};
    const double a[5]={1,-3.606129410855792,4.894109789457051,-2.961577327764605,0.674024707627429};
    //const double b[5]={449,1794,2691,1794,449};
    //const double a[5]={16777216,-60500812,82109537,-49687023,11308258};

    //Для экспоненциального фильтра
    double old_Yx3=0;
    double old_Yy3=0;
};

#endif // FIRFILTER_H
