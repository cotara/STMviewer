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
    double Yx =0;
    double dx_mas[201];
    int   dx_mas_counter =0, dx_mas_counter_offset =0;

    for(int j=0;j<len;j++){
        dx_mas[dx_mas_counter] = (unsigned char)in.at(j);
          Yx = 0 ;
          for(int i=0;i<offset;i++){
            dx_mas_counter_offset =dx_mas_counter+i;
            if (dx_mas_counter_offset>=offset)
              dx_mas_counter_offset= dx_mas_counter_offset-(offset);
            Yx=Yx+koefs[i]*dx_mas[dx_mas_counter_offset];
          }
          dx_mas_counter++;
          if (dx_mas_counter == offset)
                dx_mas_counter=0;
          if(Yx<0)
              Yx=0;
          else if(Yx>255)
              Yx=255;
          out.append((unsigned char)Yx);
   }

   return out;
}
//В зависимости от масштаба лазера (scale) необходимо менять параметры поиска экстремумов:
//1. scale*0.5 - дельта при которой фиксируется экстремум
//2. scale/10 - Во сколько раз экстремум должен быть больше среднего значения экстремумов, чтобы стать фронтом
QVector <QVector<double>> firFilter::maximumFind(QByteArray &in,int len, int scale){

    int frontIndex=0;
    unsigned char ch,chPrev;
    double mean,sumAllExtremums=0,upVal=0, downVal=0;
    QVector <double> maxmin, x,tempVect;
    QVector <QVector<double>> result;
    bool firstFind=false;
    double tempPeriod1, tempPeriod2, extrLimit=scale*0.5, frontLimit=scale/10;
    int cicleCount;
    chPrev=in.at(0);
    for(int j=1;j<len;j++){
       ch=in.at(j);
       if(ch>chPrev){
            upVal+=ch-chPrev;
            if(downVal>extrLimit && downVal<255){                             //Нашли значительный минимум
                    x.append(j-1);
                    maxmin.append(chPrev);
                    sumAllExtremums+=downVal;
                    mean=sumAllExtremums/x.size();                    //Среднее значение максимумов
             }
            downVal=0;
       }
       else if(ch<chPrev){
            downVal+=chPrev-ch;
            if(upVal>extrLimit && upVal<255){                           //Нашли максимум
                    if(firstFind==true){
                        if(upVal>mean*frontLimit){               //Если амплитуда значительно превосходит среднюю (зависит от масштаба лазера)
                           frontIndex=x.size();         //Запоминаем индекс в листе максимумов и минимумов
                        }
                        x.append(j-1);
                        maxmin.append(chPrev);
                        sumAllExtremums+=upVal;
                        mean=sumAllExtremums/x.size();
                    }
                    else
                        firstFind=true;                    //Нашли первый максимум, который надо отбросить
            }
            upVal=0;
       }
       chPrev=ch;
    }
    if(frontIndex<2 || (x.size()-1 - frontIndex)<2)                                                    //До фронта и после не найдено еще 2 экстремума, значит какая-то лажа
      qDebug() << "No extremums,fu*k! ";

    else{   //кол-во точек справа    //кол-во точек слева
        if(x.size() - frontIndex-1<frontIndex)                           //Если коилчество экстремумов справа меньше, то цикл будем делать до количества экстремумов справа
            cicleCount = x.size() - frontIndex-2;
        else
            cicleCount = frontIndex-1;
        double normPeriod = (x.at(frontIndex-1)-x.at(frontIndex-2) + x.at(frontIndex+2)-x.at(frontIndex+1))/2;     // Находим размер нормального периода(от фронта до предыдущего экстремума). От него плясать будем и находить остальные
        //Сохраняем первые максимумы слева и справа
        tempVect.append(x.at(frontIndex-1));
        tempVect.append(maxmin.at(frontIndex-1));
        result.push_back(tempVect);
        tempVect.clear();
        tempVect.append(x.at(frontIndex+1));
        tempVect.append(maxmin.at(frontIndex+1));
        result.push_back(tempVect);
        tempVect.clear();
        //Сохраняем 8 экстремумов в лист

        for(int i=0;i<cicleCount;i++){
            tempPeriod1 = x.at(frontIndex-i-1)-x.at(frontIndex-i-2);                          //Считаем расстояние до следующих экстремумов
            tempPeriod2 = x.at(frontIndex+i+2)-x.at(frontIndex+i+1);
            if(normPeriod*1.5>tempPeriod1 && normPeriod*1.5>tempPeriod2) {                           //При отдалении от фронта период уменьшается. Поэтому, если он вдруг вырос, значит экстремум не последовательный
                tempVect.append(x.at(frontIndex-i-2));
                tempVect.append(maxmin.at(frontIndex-i-2));
                result.push_back(tempVect);
                 tempVect.clear();
                tempVect.append(x.at(frontIndex+i+2));
                tempVect.append(maxmin.at(frontIndex+i+2));
                result.push_back(tempVect);
                tempVect.clear();
            }
            else
                return result;
        } 
    }
    return result;
}

QVector<double> firFilter::shadowFind(double x1, double x2 ,double x3)
{
    QVector<double> x0;
    double la = 905.0/1000000000.0;
    double L = 207.4/1000;
    double p1=2.51087470;
    double p2=0.83484861;
    double p3=0.45007122;
    double y1=la*L*L*p1/(4*(x3-x1)*(x3-x1)*0.000004*0.000004 + la*L*p1);
    double y2=la*L*L*p2/(4*(x2-x1)*(x2-x1)*0.000004*0.000004 + la*L*p2);
    double y3=la*L*L*p3/(4*(x3-x2)*(x3-x2)*0.000004*0.000004 + la*L*p3);
    double x01=x1-sqrt(la*L*(L-y1)*1.5/(2*y1))/0.000004;
    double x02=x2-sqrt(la*L*(L-y2)*3.5/(2*y2))/0.000004;
    double x03=x3-sqrt(la*L*(L-y3)*5.5/(2*y3))/0.000004;

    x0.append(x01);
    x0.append(x02);
    x0.append(x03);
    return x0;
}


/*
double firFilter::freqCalc(QList<double> dots,int len){
    double tempSum=0;
    for(int i=0;i<len;i++){
        tempSum+=dots.at(i);
    }
    return tempSum/len;
}
*/
