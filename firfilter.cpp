#include "firfilter.h"
#include <QDebug>
#include <QTextStream>


firFilter::firFilter()
{
    file =  new QFile();

    file->setFileName("koeff.txt");
    if(!file->open(QIODevice::ReadOnly)){
      qDebug()<<"Файл с коэффициентами не открывается";
      return;
    }
    QTextStream in(file);
    int k=0;
    while (!in.atEnd()) {
        QString line = in.readLine();
        koefs[k]=line.toDouble();
        k++;
    }
    offset=k;//201
}
firFilter::~firFilter(){
    delete file;
}

QByteArray firFilter::toFilter(QByteArray &in,int len){
    QByteArray out;
    /*double temp=0;
    for(int i=offset-1;i<len;i++){
        for(int j=0;j<offset;j++){
            temp=temp+koefs[j]*(double)in.at(i-j);
        }
        out.insert(i,(unsigned char)temp);
        temp=0;
    }
    return out;*/
    unsigned char dx_mas[201]={0};
    int dx_mas_counter=0,dx_mas_counter_offset=0;
    double Yx;
    for (int j=0;j<len;j++){
        dx_mas[dx_mas_counter] = in.at(j);
        Yx = koefs[0];
        for (int i = 1;i<201;i++){
           dx_mas_counter_offset = dx_mas_counter+i-1;
           if (dx_mas_counter_offset>200){
              dx_mas_counter_offset-=201;
           }
           Yx=Yx+koefs[i]*dx_mas[dx_mas_counter_offset];
         }
         dx_mas_counter++;
         if (dx_mas_counter == 201){
            dx_mas_counter=1;
         }
         out.append((unsigned char)Yx);

    }

    return out;
}
