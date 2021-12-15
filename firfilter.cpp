#include "firfilter.h"
#include <QDebug>
#include <QTextStream>
#include <cmath>
#include <QMessageBox>

firFilter::firFilter(QVector<double> &s)
{
    updateSettings(s);

    file =  new QFile();
    file->setFileName("koeff.txt");
    if(!file->open(QIODevice::ReadOnly)){
      qDebug()<<"koeff.txt сannot be opened";
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
   const double x00=static_cast<unsigned char>(in.at(0));
    const double x01=static_cast<unsigned char>(in.at(1));
    const double x02=static_cast<unsigned char>(in.at(2));
    const double x03=static_cast<unsigned char>(in.at(3));
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
          x0=  static_cast<unsigned char>(in.at(j));
          x1=  static_cast<unsigned char>(in.at(j-1));
          x2=  static_cast<unsigned char>(in.at(j-2));
          x3=  static_cast<unsigned char>(in.at(j-3));
          x4=  static_cast<unsigned char>(in.at(j-4));
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
//В один проход
QVector <QVector<unsigned int>> firFilter::extrFind2(QByteArray &in,int len){
    const int start= 200, stop = len-200;                                   //Начало и конец поиска экстремумов
    unsigned char yMin11=255,yMin12=255,yMin21=255,yMin22=255,yCurrentMin, yLastMin1, yLastMin2;          //Искомые 4 точки (Y) и промежуточные экстремумы
    unsigned int xMin11=start,xMin12=start,xMin21=start,xMin22=start,xCurrentMin,xLastMin1, xLastMin2;       //Искомые 4 точки (X)
    unsigned int  xStartStraight=0;                                         //Начало прямого участка (X)
    unsigned char ch,chPrev,up=0,down=0,upMax=0,downMax=0;                  //Текущее и предыдущее значение сигнала
    QVector <unsigned int> xMin,yMin,tempVect;                              //все минимумы и временный вектор
    QVector <QVector<unsigned int>> result;                                 //Результирующий вектор пар X,Y
    unsigned int state = 1;                                                 //0- straight, 1 = falling; 2 - rising;

    chPrev=in.at(start);                                                    // Запоминаем первую точку
    for(int j=start+1;j<stop;j++){                                          //Идем по всем точкам
       ch=in.at(j);                                                         //Берем новую точку
       if(ch==chPrev) {                                                     //Если сигнал не изменился
           if(state!=0)                                                     //Если это впервые после роста или спада
                xStartStraight=j-1;                                         //запоминаем начало прямого участка
           state=0;                                                         //сейчс прямой участок
       }
       else if(ch>chPrev){                                                  //Если текущее значение больше предыдущего, это значит, что график пошел вверх
            if(down > 1){                                                   //Если состояние невозрастания и оно достаточно (, то Нашли минимум
                yCurrentMin = chPrev;
                if(state==0)                                                //Если экстремум найден после прямого участка
                    xCurrentMin = (j-1+xStartStraight)/2;                   //Берем середину прямого участка
                else                                                        //Иначе
                    xCurrentMin=j-1;

                //Отдельный алгоритм поиска спада
                if(down>=downMax){                                          //Претендент на спад
                    downMax=down;                                           //обновили размер спада
                    yMin21 = yCurrentMin;                                   //Сохранили Y минимума
                    xMin21=xCurrentMin;                                     //Сохранили X минимума
                    yMin22=255;                                             //Обнулили Y следующего минимума (теперь мы его не знаем)
                }
                else{                                                       //Если найденный спад меньше максимального, возможно это вротой минимум после спада?
                    if(yMin22 == 255){                                      //Если после спада мы еще не находили минимумов,
                        yMin22 = yCurrentMin;                               //Сохраняем Y второго минимума после спада
                        xMin22 = xCurrentMin;                               //Сохранили X минимума
                    }
                }

                //Храним два последних минимума всю дорогу
                yLastMin1=yLastMin2;
                xLastMin1=xLastMin2;
                yLastMin2=yCurrentMin;
                xLastMin2=xCurrentMin;
            }
            up+=ch-chPrev;                                                  //Считаем на сколько вырасло
            down=0;
            state=2;                                                        //Сейчас возрастание

       }
       else if(ch<chPrev){                                                  //Если текущее значение меньше предыдущего, это значит, что график пошел вниз
           if((up > 1)){                                                    //нашли максимум
               if(up>upMax){                                                //Нашли потенциальный фронот
                   upMax=up;
                   //Обновляем актуальные два минимума слева от фронта
                   xMin11=xLastMin1;
                   yMin11=yLastMin1;
                   xMin12=xLastMin2;
                   yMin12=yLastMin2;
               }
           }
           down+=chPrev-ch;                                                   //Считаем на сколько упало
           up=0;
           state=1;                                                         //Сейчас убывание

       }
       chPrev=ch;                                                           //Сохраняем текущее значение сигнала в предыдущее
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
    QVector<double> x;
    double delta1 = dots.at(1)-dots.at(0);
    double delta2 = dots.at(3)-dots.at(2);


    double y1=la*Hx*Hx*p1/(4*delta1*delta1*res*res + la*Hx*p1);
    double y2=la*Hy*Hy*p1/(4*delta2*delta2*res*res + la*Hy*p1);


    double x01=dots.at(1)+sqrt(la*Hy*(Hy-y1)*1.5/(2*y1))/res;
    double x02=dots.at(2)-sqrt(la*Hy*(Hy-y2)*1.5/(2*y2))/res;


    x.append(x01);
    x.append(x02);

    return x;
}

QVector<double> firFilter::shadowFind(QVector<unsigned int> dots)
{
    QVector<double> x;

    double delta1 = dots.at(1)-dots.at(0);
    double delta2 = dots.at(3)-dots.at(2);

    double y1=la*Hx*Hx*p1/(4*delta1*delta1*res*res + la*Hx*p1);
    double y2=la*Hy*Hy*p1/(4*delta2*delta2*res*res + la*Hy*p1);

    double x01=dots.at(1)+sqrt(la*Hx*(Hx-y1)*1.5/(2*y1))/res;
    double x04=dots.at(2)-sqrt(la*Hy*(Hy-y2)*1.5/(2*y2))/res;

    x.append(x01);
    x.append(x04);

    return x;
}

QVector<double> firFilter::diameterFind(QVector<double> shadowsCh1, QVector<double> shadowsCh2){
    QVector<double> result;
    if(shadowsCh1.size()>1 && shadowsCh2.size()>1){
            double Front1 = shadowsCh1.at(0);
            double Spad1 = shadowsCh1.at(1);
            double Front2 = shadowsCh2.at(0);
            double Spad2 = shadowsCh2.at(1);

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
            result.append(Rx1);
            result.append(Ry1);
            result.append(Ex01-Cx);
            result.append(Ey01-Cy);
    }
    return result;
}

void firFilter::updateSettings(QVector<double> &s)
{
    if(s.size()==7){
        la = s.at(0);
        Nx = s.at(1);
        Ny = s.at(2);
        Hx = s.at(3);
        Hy = s.at(4);
        Cx = s.at(5);
        Cy = s.at(6);
    }
    else
        QMessageBox::warning(this, "Внимание!", "Настройки не были прочитаны корректно",QMessageBox::Ok);
}

QVector<double> firFilter::medianFilter(QVector<double> data, int window, int average){
    return data;
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
