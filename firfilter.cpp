#include "firfilter.h"
#include <QDebug>
#include <QTextStream>
#include <cmath>


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
    QByteArray out,outShifted;
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
                                          //Если двигаем влево
          outShifted=out.mid(100);                   //Берем с позиции n (минус т.к. n - отрицательное)
          outShifted.append(100,0);
   return outShifted;
}
/*
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
*/
QVector <QVector<double>> firFilter::extrFind(QByteArray &in,int len){
    int frontIndex=0,frontValue=0;
    unsigned char ch,chPrev;
    double upVal=0, downVal=0;
    QVector <double> maxmin, x,tempVect;
    QVector <QVector<double>> result;
    double tempPeriod1, tempPeriod2;

    chPrev=in.at(0);
    for(int j=1;j<len;j++){
       ch=in.at(j);
       if(ch==chPrev)                                               //Если значение не растет и не убывает, пропускаем
           continue;
       if(ch>chPrev){                                               //График пошел вверх
            if(abs(ch-chPrev)<100)                                  //Если это не глюк фильтра (не слишком резко)
                upVal+=ch-chPrev;                                   //Суммируем будующий максимум
            if(downVal>10){                                         //Нашли значительный минимум
                x.append(j-1);
                maxmin.append(chPrev);
            }
            downVal=0;
       }
       else if(ch<chPrev){                                          //График пошел вниз
            if(abs(ch-chPrev)<100)                                  //Если это не глюк фильтра (не слишком резко)
                downVal+=chPrev-ch;                                 //Суммируем будующий максимум
            if(upVal>10){                                          //Нашли максимум
                if(chPrev>frontValue){                             //Запоминаем наибольший максимум (потенциальный фронт)
                   frontIndex=x.size();                            //Запоминаем индекс в листе максимумов и минимумов
                   frontValue=chPrev;                              //Запоминаем значение максимума, как фронт
                }
                x.append(j-1);
                maxmin.append(chPrev);
            }
            upVal=0;
       }
       chPrev=ch;
    }
    if(frontIndex<3 || (x.size()-1 - frontIndex)<3)               //Ищем 3 экстремума до и после
      qDebug() << "not enough (4) extremes!!!";
    else{
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
        //Сохраняем остальные 4 экстремума в лист
        for(int i=0;i<2;i++){
            tempPeriod1 = x.at(frontIndex-i-1)-x.at(frontIndex-i-2);                          //Считаем расстояние до следующих экстремумов
            tempPeriod2 = x.at(frontIndex+i+2)-x.at(frontIndex+i+1);
            tempVect.append(x.at(frontIndex-i-2));
            tempVect.append(maxmin.at(frontIndex-i-2));
            result.push_back(tempVect);
            tempVect.clear();
            tempVect.append(x.at(frontIndex+i+2));
            tempVect.append(maxmin.at(frontIndex+i+2));
            result.push_back(tempVect);
            tempVect.clear();
            if(normPeriod*1.5<tempPeriod1 && normPeriod*1.5<tempPeriod2)                     //При отдалении от фронта период уменьшается. Поэтому, если он вдруг вырос, значит экстремум не последовательный
                qDebug() << "Invalid Extremes #" + QString::number(i+2);
        }
    }
    return result;
}

//Первые три - левые тени, вторые три - правые тени
QVector<double> firFilter::shadowFind(QVector<double> dots)
{
    QVector<double> x0;
    double la = 905.0/1000000000.0;
    double L = 207.4/1000;
    double p1=2.51087470;
    double p2=0.83484861;
    double p3=0.45007122;
    double res=0.000004;
    double delta1 = dots.at(0)-dots.at(4);
    double delta2 = dots.at(0)-dots.at(2);
    double delta3 = dots.at(2)-dots.at(4);
    double delta4 = dots.at(5)-dots.at(1);
    double delta5 = dots.at(3)-dots.at(1);
    double delta6 = dots.at(5)-dots.at(3);
    double y1=la*L*L*p1/(4*delta1*delta1*res*res + la*L*p1);
    double y2=la*L*L*p2/(4*delta2*delta2*res*res + la*L*p2);
    double y3=la*L*L*p3/(4*delta3*delta3*res*res + la*L*p3);
    double y4=la*L*L*p1/(4*delta4*delta4*res*res + la*L*p1);
    double y5=la*L*L*p2/(4*delta5*delta5*res*res + la*L*p2);
    double y6=la*L*L*p3/(4*delta6*delta6*res*res + la*L*p3);
    double x01=dots.at(0)+sqrt(la*L*(L-y1)*1.5/(2*y1))/res;
    double x02=dots.at(2)+sqrt(la*L*(L-y2)*3.5/(2*y2))/res;
    double x03=dots.at(4)+sqrt(la*L*(L-y3)*5.5/(2*y3))/res;
    double x04=dots.at(1)-sqrt(la*L*(L-y4)*1.5/(2*y4))/res;
    double x05=dots.at(3)-sqrt(la*L*(L-y5)*3.5/(2*y5))/res;
    double x06=dots.at(5)-sqrt(la*L*(L-y6)*5.5/(2*y6))/res;

    x0.append(x01);
    x0.append(x02);
    x0.append(x03);
    x0.append(x04);
    x0.append(x05);
    x0.append(x06);
    return x0;
}

QVector<double> firFilter::diameterFind(QVector<QVector<double>> shadowsCh1, QVector<QVector<double> > shadowsCh2){
    QVector<double> result;

    const double Nx=5320;
    const double Ny=5320;
    const double Hx=207.4;
    const double Hy=207.4;
    const double Cx=73.4;
    const double Cy=73.4;
    const double res =  0.004;
    if(shadowsCh1.size()>5 && shadowsCh2.size()>5){
        for(int i=0;i<3;i++){

            double Front1 = shadowsCh1.at(i).at(0);
            double Spad1 = shadowsCh1.at(i+3).at(0);
            double Front2 = shadowsCh2.at(i).at(0);
            double Spad2 = shadowsCh2.at(i+3).at(0);

            double  X11 = (-Front1+ Nx)*res+Cx;
            double  X21 = (-Spad1+ Nx)*res+Cx;
            double  Y11 = (-Front2+Ny)*res+Cy;
            double  Y21 = (-Spad2+Ny)*res+Cy;

            double  X01 =    Hx*tan(0.5*(atan((X21-Cx)/Hx)+atan((X11-Cx)/Hx)))+Cx ;
            double  Y01 =    Hy*tan(0.5*(atan((Y21-Cy)/Hy)+atan((Y11-Cy)/Hy)))+Cy ;

            double  Ex01 =(Cx*Hy*Y01 + Hx*Hy*X01 - Hy*X01*Y01)/(Hx*Hy - Cx*Cy - X01*Y01 + Cy*X01 + Cx*Y01);
            double  Ey01 =(Hx*Cy*X01 + Hx*Hy*Y01 - Hx*X01*Y01)/(Hx*Hy - Cx*Cy - X01*Y01 + Cy*X01 + Cx*Y01);

            double Rx1 = sqrt((Ex01-Cx)*(Ex01-Cx)+(Hx-Ey01)*(Hx-Ey01))*sin(0.5*(-atan((X21-Cx)/Hx)+atan((X11-Cx)/Hx)));
            double Ry1 = sqrt((Ey01-Cy)*(Ey01-Cy)+(Hy-Ex01)*(Hy-Ex01))*sin(0.5*(-atan((Y21-Cy)/Hy)+atan((Y11-Cy)/Hy)));

            result.append(Rx1 + Ry1);
        }
    }
    return result;
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
