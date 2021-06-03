#ifndef FIRFILTER_H
#define FIRFILTER_H
#include "qfile.h"

class firFilter
{
public:
    firFilter();
    ~firFilter();
    QByteArray toFilter(QByteArray &in,int len);

private:
    QFile *file;
    double koefs[201];
    int offset=0;
};

#endif // FIRFILTER_H
