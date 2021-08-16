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


QByteArray firFilter::toButterFilter(QByteArray &in,int len){
    QByteArray out,outShifted;
   const double x00=(unsigned char)in.at(0);
    const double x01=(unsigned char)in.at(1);
    const double x02=(unsigned char)in.at(2);
    const double x03=(unsigned char)in.at(3);
    /*
    const short x00=(unsigned char)in.at(0);
    const short x01=(unsigned char)in.at(1);
    const short x02=(unsigned char)in.at(2);
    const short x03=(unsigned char)in.at(3);
    */
    double x0,x1,x2,x3,x4,y1,y2,y3,y4;
    QVector<double>yOut={x00,x01,x02,x03};
    //QVector<short>yOut={x00,x01,x02,x03};
    double Yx =0;
    for(int j=4;j<len;j++){
          x0=  (unsigned char)in.at(j);
          x1=  (unsigned char)in.at(j-1);
          x2=  (unsigned char)in.at(j-2);
          x3=  (unsigned char)in.at(j-3);
          x4=  (unsigned char)in.at(j-4);
          y1=  yOut.at(j-1);
          y2=  yOut.at(j-2);
          y3=  yOut.at(j-3);
          y4=  yOut.at(j-4);

          Yx = b[0]*x0 +  b[1]*x1 + b[2]*x2 + b[3]*x3 + b[4]*x4 - a[1]*y1 - a[2]*y2 - a[3]*y3 - a[4]*y4;
          yOut.append(Yx);

          if(Yx<0)
              Yx=0;
          else if(Yx>255)
              Yx=255;
          out.append((unsigned char)Yx);
   }
          outShifted=out.mid(15);
          outShifted.append(15,0);
   return outShifted;
}

QVector <QVector<unsigned int>> firFilter::extrFind2(QByteArray &in,int len){
    const unsigned int start= 100, stop = len-100;                          //Начало и конец поиска экстремумов
    int  backMinsIndex=0;                                                   //индекс спада в векторе минимумов
    unsigned char yMin11=0,yMin12=0,yMin21=0,yMin22=0,yCheckPoint;          //Искомые 4 точки (Y) и промежуточные экстремумы
    unsigned int xMin11=start,xMin12=start,xMin21=start,xMin22=start;       //Искомые 4 точки (X)
    unsigned int  xStartStraight=0;                                         //Начало прямого участка (X)
    unsigned char yFront=30, yBack=30;                                               //фронт (Y)
    unsigned char ch,chPrev,up=0,down=0;                                                //Текущее и предыдущее значение сигнала
    QVector <unsigned int> xMin,yMin,tempVect;                              //все минимумы и временный вектор
    QVector <QVector<unsigned int>> result;                                 //Результирующий вектор пар X,Y
    unsigned int state = 1;                                                 //0- straight, 1 = falling; 2 - rising;

    chPrev=in.at(start);                                                    // Запоминаем первую точку
    yCheckPoint=chPrev;                                                     //Запоминаем первый псевдоэкстемум
    for(int j=start+1;j<stop;j++){                                          //Идем по всем точкам
       ch=in.at(j);                                                         //Берем новую точку
       if(ch==chPrev) {                                                     //Если сигнал не изменился
           if(state!=0)                                                     //Если это впервые после роста или спада
                xStartStraight=j-1;                                         //запоминаем начало прямого участка
           state=0;                                                         //сейчс прямой участок
       }
       else if(ch>chPrev){                                                  //Если текущее значение больше предыдущего, это значит, что график пошел вверх
            if((state!=2) && (down > 3)){                                   //Если состояние невозрастания и оно достаточно (7), то Нашли минимум
                yMin.append(chPrev);                                        //Добавляем его в массив
                if(state==0)                                                //Если экстремум найден после прямого участка
                    xMin.append((j-1+xStartStraight)/2);                    //Берем середину прямого участка
                else                                                        //Иначе
                    xMin.append(j-1);                                       //саму точку
                if(down>=yBack){                                            //Если найденный минимум меньше последнего, то запоминаем, как потенциальный спад
                    yBack=xMin.last();
                    backMinsIndex=xMin.size()-1;
                }
            }
            up+=ch-chPrev;                                                  //Считаем на сколько вырасло
            down=0;
            state=2;                                                        //Сейчас возрастание

       }
       else if(ch<chPrev){                                                  //Если текущее значение меньше предыдущего, это значит, что график пошел вниз
           if((state!=1) && (up > 3)){                                      //Если состояние непадения и оно достаточно (7), то нашли максимум
               if(up>yFront){                                                   //Если найденный максимум больше последнего, то запоминаем, как потенциальный фронт
                   yFront=chPrev;
                   if(xMin.size()>1){                                           //Обновляем актуальные два минимума слева от фронта
                       xMin11=xMin.at(xMin.size()-2);
                       yMin11=yMin.at(xMin.size()-2);
                       xMin12=xMin.at(xMin.size()-1);
                       yMin12=yMin.at(xMin.size()-1);
                   }
               }
           }
           down+=chPrev-ch;                                                   //Считаем на сколько упало
           up=0;
           state=1;                                                         //Сейчас убывание

       }
       chPrev=ch;                                                           //Сохраняем текущее значение сигнала в предыдущее
    }
    //После прохода понятно, где фронт, а значит и где правые минимумы



    if(xMin.size() > backMinsIndex+2){                                      //Проверяем, что после левых нашли еще минимумы (4??)
        xMin21=xMin.at(backMinsIndex);
        yMin21=yMin.at(backMinsIndex);
        xMin22=xMin.at(backMinsIndex+1);
        yMin22=yMin.at(backMinsIndex+1);

    }
    //Собираем 4 точки по векторам
    tempVect.append(xMin11);
    tempVect.append(yMin11);
    result.push_back(tempVect);
    tempVect.clear();
    tempVect.append(xMin12);
    tempVect.append(yMin12);
    result.push_back(tempVect);
    tempVect.clear();
    tempVect.append(xMin21);
    tempVect.append(yMin21);
    result.push_back(tempVect);
    tempVect.clear();
    tempVect.append(xMin22);
    tempVect.append(yMin22);
    result.push_back(tempVect);
    return result;
}



QVector <QVector<double>> firFilter::extrFind(QByteArray &in,int len){
    int frontIndex=0,frontValue=0,straightCount=0;
    unsigned char ch,chPrev;
    double upVal=0, downVal=0;
    QVector <double> maxmin, x,tempVect;
    QVector <QVector<double>> result;
    double tempPeriod1, tempPeriod2;

    chPrev=in.at(0);                                                 // Запоминаем первую точку
    for(int j=1;j<len;j++){                                          //Обход начинаем со второй
       ch=in.at(j);                                                  //Сохраняем первую точку в переменную ch
       if(ch==chPrev){                                               //Если значение не растет и не убывает
           straightCount++;                                          //Увеличиваем соответствующий счетчик
           continue;                                                 //Пропускаем шаг цикла
       }
       if(ch>chPrev){                                               //Если текущее значение больше предыдущего, это значит, что график пошел вверх
            if(abs(ch-chPrev)<100)                                  //Проверка на резкий скачок фильтра в начале (Если это не глюк фильтра (не слишком резко))
                upVal+=ch-chPrev;                                   //Суммируем на сколько вырос график
            if(downVal>7){                                          //Если это первое возрастание после убывания (downVal!=0), и величина убывания больше некоторого значения (7), это значит, что найден минимум
                x.append(j-1-straightCount/2);                      //Запоминаем x координату минимума. (j-1-straightCount/2) означает размещение точки посередине пярмого участка, если он есть
                maxmin.append(chPrev);                              //Запоминаем занчением минимума
            }
            downVal=0;                                              //Обнуляем значение убывания графика
            straightCount=0;                                        //Обнуляем счетчик прямого движения графика
       }
       else if(ch<chPrev){                                          //Если текущее значение меньше предыдущего, это значит, что график пошел вниз
            if(abs(ch-chPrev)<100)                                  //Проверка на резкий скачок фильтра в начале (Если это не глюк фильтра (не слишком резко))
                downVal+=chPrev-ch;                                 //Суммируем на сколько опустился график
            if(upVal>7){                                            //Если это первое убывание после возрастания (upVal!=0), и величина возрастания больше некоторого значения (7), это значит, что найден максимум
                if(chPrev>frontValue){                              //Если найденный максимум больше, чем frontValue - значение потенциального фронта
                   if(j<10000 && j>500){
                    frontIndex=x.size();                             //Запоминаем индекс нового фронта
                    frontValue=chPrev;                               //Запоминаем значение нового фронта
                   }
                }
                x.append(j-1-straightCount/2);                      //Запоминаем x координату максимума. (j-1-straightCount/2) означает размещение точки посередине пярмого участка, если он есть
                maxmin.append(chPrev);                              //Запоминаем занчением максимума
            }
            upVal=0;                                                //Обнуляем значение возрастания графика
            straightCount=0;                                        //Обнуляем счетчик прямого движения графика
       }
       chPrev=ch;                                                   //Сохраняем текущее значение в предыдущее
    }
    //Постобработка
    //Ищем всплески на плато
    //У нас в наличии уже есть координата фронта и его з начение
    if(frontIndex+1<maxmin.size()){
        if (maxmin.at(frontIndex)-maxmin.at(frontIndex+1) < 20){        //Если кочка (минимум) после фронта отличается не более чем на 20 единиц от фронта, то это не нужный экстремум, а фейковый всвлеск
            x.remove(frontIndex+1,2);                                   //Удаляем два следующих экстремума (минимум и максимум)
            maxmin.remove(frontIndex+1,2);
        }
    }

    //Выделяем из всех найденных экстремумов только 3 до фронта и 3 после фронта
    //Проверяем, если 3 экстремумов до или после нет, то выводим ошибку, что экстремумов недостаточно.
    if(frontIndex<3 || (x.size()-1 - frontIndex)<3)
      qDebug() << "not enough extremes!!!";
    else{                                                                                                           //Здесь алгоритм, который позволяет сохранить именно 3 подряд идущих экстремумов
        double normPeriod = (x.at(frontIndex-1)-x.at(frontIndex-2) + x.at(frontIndex+2)-x.at(frontIndex+1))/2;     // Находим размер нормального периода(от фронта до предыдущего экстремума).
        //Сохраняем первые максимумы слева и справа
        //Ниже процесс формирования двумерного массива из координаты и значения экстремума
        tempVect.append(x.at(frontIndex-1));
        tempVect.append(maxmin.at(frontIndex-1));
        result.push_back(tempVect);
        tempVect.clear();
        tempVect.append(x.at(frontIndex+1));
        tempVect.append(maxmin.at(frontIndex+1));
        result.push_back(tempVect);
        tempVect.clear();
        //Сохраняем остальные 4 экстремума в двумерный массив
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
            if(normPeriod*1.5<tempPeriod1 && normPeriod*1.5<tempPeriod2)                     //Если текущий экстремум отдален от предыдущего на величину в 1,5 раз большую, чем normPeriod, то считаем этот экстремум не последовательным
                qDebug() << "Invalid Extremes #" + QString::number(i+2);
        }
    }
    return result;
}

//находит левую и правую тень для одного канала
QVector<double> firFilter::shadowFind(QVector<double> dots){
    QVector<double> x0;

    double delta1 = dots.at(0)-dots.at(4);
    double delta4 = dots.at(5)-dots.at(1);
    //double delta2 = dots.at(0)-dots.at(2);
    //double delta3 = dots.at(2)-dots.at(4);

    //double delta5 = dots.at(3)-dots.at(1);
    //double delta6 = dots.at(5)-dots.at(3);

    double y1=la*L*L*p1/(4*delta1*delta1*res*res + la*L*p1);
    double y4=la*L*L*p1/(4*delta4*delta4*res*res + la*L*p1);
    //double y2=la*L*L*p2/(4*delta2*delta2*res*res + la*L*p2);
    //double y3=la*L*L*p3/(4*delta3*delta3*res*res + la*L*p3);
    //double y5=la*L*L*p2/(4*delta5*delta5*res*res + la*L*p2);
    //double y6=la*L*L*p3/(4*delta6*delta6*res*res + la*L*p3);

    double x01=dots.at(0)+sqrt(la*L*(L-y1)*1.5/(2*y1))/res;
    double x04=dots.at(1)-sqrt(la*L*(L-y4)*1.5/(2*y4))/res;
    //double x02=dots.at(2)+sqrt(la*L*(L-y2)*3.5/(2*y2))/res;
    //double x03=dots.at(4)+sqrt(la*L*(L-y3)*5.5/(2*y3))/res;
    //double x05=dots.at(3)-sqrt(la*L*(L-y5)*3.5/(2*y5))/res;
    //double x06=dots.at(5)-sqrt(la*L*(L-y6)*5.5/(2*y6))/res;

    x0.append(x01);
    x0.append(x04);
    //x0.append(x02);
    //x0.append(x03);
    //x0.append(x05);
    //x0.append(x06);
    return x0;
}

QVector<double> firFilter::shadowFind(QVector<unsigned int> dots)
{
    QVector<double> x0;

    double delta1 = dots.at(1)-dots.at(0);
    double delta4 = dots.at(3)-dots.at(2);

    double y1=la*L*L*p1/(4*delta1*delta1*res*res + la*L*p1);
    double y4=la*L*L*p1/(4*delta4*delta4*res*res + la*L*p1);

    double x01=dots.at(1)+sqrt(la*L*(L-y1)*1.5/(2*y1))/res;
    double x04=dots.at(2)-sqrt(la*L*(L-y4)*1.5/(2*y4))/res;

    x0.append(x01);
    x0.append(x04);

    return x0;
}

QVector<double> firFilter::diameterFind(QVector<QVector<double>> shadowsCh1, QVector<QVector<double> > shadowsCh2){
    QVector<double> result;
    if(shadowsCh1.size()>1 && shadowsCh2.size()>1){
            double Front1 = shadowsCh1.at(0).at(0);
            double Spad1 = shadowsCh1.at(1).at(0);
            double Front2 = shadowsCh2.at(0).at(0);
            double Spad2 = shadowsCh2.at(1).at(0);

            double  X11 = (-Front1+ Nx)*res2+Cx;
            double  X21 = (-Spad1+ Nx)*res2+Cx;
            double  Y11 = (-Front2+Ny)*res2+Cy;
            double  Y21 = (-Spad2+Ny)*res2+Cy;

            double  X01 =    Hx*tan(0.5*(atan((X21-Cx)/Hx)+atan((X11-Cx)/Hx)))+Cx ;
            double  Y01 =    Hy*tan(0.5*(atan((Y21-Cy)/Hy)+atan((Y11-Cy)/Hy)))+Cy ;

            double  Ex01 =(Cx*Hy*Y01 + Hx*Hy*X01 - Hy*X01*Y01)/(Hx*Hy - Cx*Cy - X01*Y01 + Cy*X01 + Cx*Y01);
            double  Ey01 =(Hx*Cy*X01 + Hx*Hy*Y01 - Hx*X01*Y01)/(Hx*Hy - Cx*Cy - X01*Y01 + Cy*X01 + Cx*Y01);

            double Rx1 = sqrt((Ex01-Cx)*(Ex01-Cx)+(Hx-Ey01)*(Hx-Ey01))*sin(0.5*(-atan((X21-Cx)/Hx)+atan((X11-Cx)/Hx)));
            double Ry1 = sqrt((Ey01-Cy)*(Ey01-Cy)+(Hy-Ex01)*(Hy-Ex01))*sin(0.5*(-atan((Y21-Cy)/Hy)+atan((Y11-Cy)/Hy)));
            result.append(Rx1 + Ry1);
            result.append(Ex01-Cx);
            result.append(Ey01-Cy);
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
