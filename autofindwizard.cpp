#include "autofindwizard.h"
#include "ui_autofindwizard.h"
#include <cmath>
#include <omp.h>
#include <qdebug.h>
#include <QMessageBox>
AutoFindWizard::AutoFindWizard(QWidget *parent, QVector<double> params) :
    QWizard(parent),
    ui(new Ui::AutoFindWizard)
{
    ui->setupUi(this);
    layout1 = new QVBoxLayout();
    catchData = new catchDataDialog(this);

    ui->wizardPage1->setLayout(layout1);
    layout1->addWidget(catchData);


    init(params);
    connect(this,&AutoFindWizard::setProgress,ui->progressBar,&QProgressBar::setValue);             //Прорессбар
    connect(ui->etalonDiameterSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),        //Изменение эталонного диаметра, как радиус
                [=](double i){ etalonMkm = static_cast<int>(500*i);});

    etalonMkm = static_cast<int>(500*ui->etalonDiameterSpinBox->value());                           //Получаем радиус на старте

    connect(catchData,&catchDataDialog::dataCatched,[=]{dataCatched=true;});                        //Получаем сигнал, что все данные собраны
    connect(catchData,&catchDataDialog::pointCatched,[=](QVector<double> data, int i){
        allExtremums[i-1] = data;});                                                            //

    connect(ui->mkSendButton,&QPushButton::clicked,[=]{
        on_pushButton_2_clicked(); //Приняли параметры
        emit sendBestParameters();//Сигнал об отправке подобранных параметров в МК
    });


}

void AutoFindWizard::init(QVector<double> params)
{
    m_params=params;
    //устанавливаем текущие параметры
    if(params.size() == 7){
        ui->laLabel->setText(QString::number(params.at(0),'g',10));
        ui->NxLabel->setText(QString::number(params.at(1),'g',10));
        ui->NyLabel->setText(QString::number(params.at(2),'g',10));
        ui->HxLabel->setText(QString::number(params.at(3),'g',10));
        ui->HyLabel->setText(QString::number(params.at(4),'g',10));
        ui->CxLabel->setText(QString::number(params.at(5),'g',10));
        ui->CyLabel->setText(QString::number(params.at(6),'g',10));
    }
    else{
        ui->laLabel->clear();
        ui->NxLabel->clear();
        ui->NyLabel->clear();
        ui->HxLabel->clear();
        ui->HyLabel->clear();
        ui->CxLabel->clear();
        ui->CyLabel->clear();
    }
    //Чистим поля подобранных параметров
    ui->laSpinBox_2->clear();
    ui->NxSpinBox_2->clear();
    ui->NySpinBox_2->clear();
    ui->HxSpinBox_2->clear();
    ui->HySpinBox_2->clear();
    ui->CxSpinBox_2->clear();
    ui->CySpinBox_2->clear();

    ui->resultDiameter->clear();
    ui->bestErr->clear();
    ui->progressBar->reset();
    ui->startFind->setChecked(false);
    ui->startFind->setChecked(false);

    catchData->clear();         //Очистка цветов и текста кнопок захвата данных
    restart();                  //Перезапуск мастера
    allExtremums.clear();
    allExtremums.resize(9);
    dataCatched=false;
    ui->startFind->setText("Подобрать");

    allFronts.clear();
    allFronts.resize(9);
    //Для теста:
    QVector <double> temp;
//    temp = {3865,6185,3865,6185};
//    allFronts[0]=temp;

//    temp = {380,2707,3942,6108};
//    allFronts[1]=temp;

//    temp = {7343,9670,3776,6274};
//    allFronts[2]=temp;

//    temp = {688,2860,688,2860};
//    allFronts[3]=temp;

//    temp = {3942,6108,380,2707};
//    allFronts[4]=temp;

//    temp = {7190,9362,25,2530};
//    allFronts[5]=temp;

//    temp = {25,2530,7190,9362};
//    allFronts[6]=temp;

//    temp = {3776,6274,7343,9670};
//    allFronts[7]=temp;

//    temp = {7520,10025,7520,10025};
//    allFronts[8]=temp;
        temp = {6704 ,8853 ,5807 ,7927 };
        allFronts[0]=temp;

        temp = {4329 ,6472 ,6836 ,9071 };
        allFronts[1]=temp;

        temp = {2559 ,4694 ,7060 ,9392 };
        allFronts[2]=temp;

        temp = {897 ,3168 ,3942 ,6370 };
        allFronts[3]=temp;

        temp = {2402 ,4790 ,1697 ,4015 };
        allFronts[4]=temp;

        temp = {4668 ,7054 ,1418 ,3635 };
        allFronts[5]=temp;

        temp = {6784 ,9121 ,2104 ,4239 };
        allFronts[6]=temp;

        temp = {7747,9975 ,4203 ,6291 };
        allFronts[7]=temp;

        temp = {4635,6883,4558,6783};
        allFronts[8]=temp;
    dataCatched=true;

}

AutoFindWizard::~AutoFindWizard(){
    delete ui;
}
//Изменение страницы визарда
void AutoFindWizard::on_AutoFindWizard_currentIdChanged(int id){
   if(id == 1) {                    //Мы перешли на страницу подбора

   }
}

//Переопределенный Finish в визарде
void AutoFindWizard::accept(){
    catchData->clear();
    QDialog::accept();
    stopPressed=true;
}

void AutoFindWizard::reject(){
    catchData->clear();
    QDialog::reject();
    stopPressed=true;
}
void AutoFindWizard::closeEvent(QCloseEvent* iEvent)
{
  // ignore close event
  //iEvent->ignore();
    catchData->clear();
    QDialog::reject();
    stopPressed=true;
}
//Алгоритм подбора параметров
void AutoFindWizard::autoFindAlg()
{
    if(allExtremums.size()!=9)
        return;
    //Начинаем поиск только если в наличии 9 валидных точек
    initla = m_params.at(0);
    initNx = m_params.at(1);
    initNy = m_params.at(2);
    initHx = m_params.at(3);
    initHy = m_params.at(4);
    initCx = m_params.at(5);
    initCy = m_params.at(6);

    //Устанавливем начальные значения для пребора
    initla *= (100-ui->laRange->value())/100.0;
    initNx *= (100-ui->NxRange->value())/100.0;
    initNy *= (100-ui->NyRange->value())/100.0;
    initHx *= (100-ui->HxRange->value())/100.0;
    initHy *= (100-ui->HyRange->value())/100.0;
    initCx *= (100-ui->CxRange->value())/100.0;
    initCy *= (100-ui->CyRange->value())/100.0;

    //Рассчитываем шаг перебора (1%)
    dla = m_params.at(0)/100;
    dNx = m_params.at(1)/100;
    dNy = m_params.at(2)/100;
    dHx = m_params.at(3)/100;
    dHy = m_params.at(4)/100;
    dCx = m_params.at(5)/100;
    dCy = m_params.at(6)/100;
    double err=0,bestErr=10000;
    steps = (ui->laRange->value()*2+1)*
            (ui->NxRange->value()*2+1)* (ui->NyRange->value()*2+1)*(ui->HxRange->value()*2+1)*
            (ui->HyRange->value()*2+1)* (ui->CxRange->value()*2+1)*(ui->CxRange->value()*2+1);
    steps/=100;
    currentStep = 0;


    //Создаем векторы значений для перебора
    laV.resize((ui->laRange->value())*2+1);
    NxV.resize((ui->NxRange->value())*2+1);
    NyV.resize((ui->NyRange->value())*2+1);
    HxV.resize((ui->HxRange->value())*2+1);
    HyV.resize((ui->HyRange->value())*2+1);
    CxV.resize((ui->CxRange->value())*2+1);
    CyV.resize((ui->CyRange->value())*2+1);

    for (int tmp = 0;tmp<laV.size();tmp++)
        laV[tmp] = initla + dla*tmp;
    for (int tmp = 0;tmp<NxV.size();tmp++)
        NxV[tmp] = initNx + dNx*tmp;
    for (int tmp = 0;tmp<NyV.size();tmp++)
        NyV[tmp] = initNy + dNy*tmp;
    for (int tmp = 0;tmp<HxV.size();tmp++)
        HxV[tmp] = initHx + dHx*tmp;
    for (int tmp = 0;tmp<HyV.size();tmp++)
        HyV[tmp] = initHy + dHy*tmp;
    for (int tmp = 0;tmp<CxV.size();tmp++)
        CxV[tmp] = initCx + dCx*tmp;
    for (int tmp = 0;tmp<CyV.size();tmp++)
        CyV[tmp] = initCy + dCy*tmp;

//Проверка, что распараллеливание подгрузилось
    #if defined(_OPENMP)
      qDebug() << "Compiled by an OpenMP-compliant implementation.\n";
    #endif
        //omp_set_num_threads(2);
      double start;
      double end;
      start = omp_get_wtime();


//#pragma omp parallel  private(err,ila,iNx,iNy, iHx,iHy,iCx,iCy)
   //{
    #pragma omp parallel for private(err,ila,iNx,iNy, iHx,iHy,iCx,iCy) //nowait//ordered //schedule(static)
    for ( iCy = 0;iCy<(ui->CyRange->value())*2+1;iCy++){
        for ( iCx = 0;iCx<(ui->CxRange->value())*2+1;iCx++){
            for ( iHy = 0;iHy<(ui->HyRange->value())*2+1;iHy++){
                for ( iHx = 0;iHx<(ui->HxRange->value())*2+1;iHx++){
                    for ( iNy = 0;iNy<(ui->NyRange->value())*2+1;iNy++){
                        for ( iNx = 0;iNx<(ui->NxRange->value())*2+1;iNx++){
                            for ( ila = 0;ila<(ui->laRange->value())*2+1;ila++){
                                err = calcErrDiemeter(ila,iNx,iNy,iHx,iHy,iCx,iCy);
                                #pragma omp critical                //В любой момент времени код критической секции может быть выполнен только одной нитью.
                                currentStep++;
                                if(err<bestErr){
                                    bestErr = err;
                                    bestla = laV[ila];
                                    bestNx = NxV[iNx];
                                    bestNy = NyV[iNy];
                                    bestHx = HxV[iHx];
                                    bestHy = HyV[iHy];
                                    bestCx = CxV[iCx];
                                    bestCy = CyV[iCy];
                                }

                                if(err<1.5){
                                    qDebug() << " Ошибка: " << err;
                                    qDebug() << " NxV: " << NxV[iNx];
                                    qDebug() << " NyV: " << NyV[iNy];
                                    qDebug() << " HxV: " << HxV[iHx];
                                    qDebug() << " HyV: " << HyV[iHy];
                                    qDebug() << " CxV: " << CxV[iCx];
                                    qDebug() << " CyV: " << CyV[iCy];
                                    qDebug() << " \n";
                                }
                                if(stopPressed){
                                    ila = ui->laRange->value()*2+1;
                                    iNx = ui->NxRange->value()*2+1;
                                    iNy = ui->NyRange->value()*2+1;
                                    iHx = ui->HxRange->value()*2+1;
                                    iHy = ui->HyRange->value()*2+1;
                                    iCx = ui->CxRange->value()*2+1;
                                    iCy = ui->CyRange->value()*2+1;
                                    currentStep = steps*100;
                                }
                            }
                        }
                    }
                    QApplication::processEvents();
                    emit setProgress(currentStep/steps);
                }
            }
        }
    }
  //}
    qDebug() << " Количество потоков: " << omp_get_num_threads();
    end = omp_get_wtime();
    qDebug() << "work-time: " << end-start;
    ui->laSpinBox_2->setValue(bestla);
    ui->NxSpinBox_2->setValue(bestNx);
    ui->NySpinBox_2->setValue(bestNy);
    ui->HxSpinBox_2->setValue(bestHx);
    ui->HySpinBox_2->setValue(bestHy);
    ui->CxSpinBox_2->setValue(bestCx);
    ui->CySpinBox_2->setValue(bestCy);
    ui->resultDiameter->setText(QString::number(ui->etalonDiameterSpinBox->value()+bestErr/1000,'g',10));
    ui->bestErr->setText(QString::number(bestErr,'g',10));
}

//Расчет радиусов по пачке экстремумов и параметров
QVector<double> AutoFindWizard::calcDiemeter(QVector<double> dots, int ila,int iNx,int iNy,int iHx,int iHy,int iCx,int iCy){
    QVector<double> x;
    double la = laV[ila];
    double Nx = NxV[iNx];
    double Ny = NyV[iNy];
    double Hx = HxV[iHx];
    double Hy = HyV[iHy];
    double Cx = CxV[iCx];
    double Cy = CyV[iCy];

    double delta1 = dots.at(1)-dots.at(0);
    double delta2 = dots.at(3)-dots.at(2);
    double delta3 = dots.at(5)-dots.at(4);
    double delta4 = dots.at(7)-dots.at(6);

    double y1=la*Hx*Hx*p1/(4*delta1*delta1*res*res + la*Hx*p1);
    double y2=la*Hy*Hy*p1/(4*delta2*delta2*res*res + la*Hy*p1);
    double y3=la*Hx*Hx*p1/(4*delta3*delta3*res*res + la*Hx*p1);
    double y4=la*Hy*Hy*p1/(4*delta4*delta4*res*res + la*Hy*p1);

    double x01=dots.at(1)+sqrt(la*Hx*(Hx-y1)*1.5/(2*y1))/res; //Левая тень
    double x02=dots.at(2)-sqrt(la*Hy*(Hy-y2)*1.5/(2*y2))/res; //Правая тень

    double x03=dots.at(5)+sqrt(la*Hx*(Hx-y3)*1.5/(2*y3))/res;
    double x04=dots.at(6)-sqrt(la*Hy*(Hy-y4)*1.5/(2*y4))/res;

    double Front1 = x01;
    double Spad1 = x02;
    double Front2 = x03;
    double Spad2 = x04;

    //для данных с экселя
    double  X11 = -(-Front1+ Nx)*res+Cx;
    double  X21 = -(-Spad1+ Nx)*res+Cx;
    double  Y11 = -(-Front2+Ny)*res+Cy;
    double  Y21 = -(-Spad2+Ny)*res+Cy;

    //Для реальных данных
    //    double  X11 = (-Front1+ Nx)*res+Cx;
    //    double  X21 = (-Spad1+ Nx)*res+Cx;
    //    double  Y11 = (-Front2+Ny)*res+Cy;
    //    double  Y21 = (-Spad2+Ny)*res+Cy;

    double  X01 =    Hx*tan(0.5*(atan((X21-Cx)/Hx)+atan((X11-Cx)/Hx)))+Cx ;
    double  Y01 =    Hy*tan(0.5*(atan((Y21-Cy)/Hy)+atan((Y11-Cy)/Hy)))+Cy ;

    double  Ex01 =(Cx*Hy*Y01 + Hx*Hy*X01 - Hy*X01*Y01)/(Hx*Hy - Cx*Cy - X01*Y01 + Cy*X01 + Cx*Y01);
    double  Ey01 =(Hx*Cy*X01 + Hx*Hy*Y01 - Hx*X01*Y01)/(Hx*Hy - Cx*Cy - X01*Y01 + Cy*X01 + Cx*Y01);

    //Для данных с эксельки
    double Rx1 = sqrt((Ex01-Cx)*(Ex01-Cx)+(Hx-Ey01)*(Hx-Ey01))*sin(0.5*(atan((X21-Cx)/Hx)-atan((X11-Cx)/Hx)));
    double Ry1 = sqrt((Ey01-Cy)*(Ey01-Cy)+(Hy-Ex01)*(Hy-Ex01))*sin(0.5*(atan((Y21-Cy)/Hy)-atan((Y11-Cy)/Hy)));

    //Для реальных данных
    //double Rx1 = sqrt((Ex01-Cx)*(Ex01-Cx)+(Hx-Ey01)*(Hx-Ey01))*sin(0.5*(-atan((X21-Cx)/Hx)+atan((X11-Cx)/Hx)));
    //double Ry1 = sqrt((Ey01-Cy)*(Ey01-Cy)+(Hy-Ex01)*(Hy-Ex01))*sin(0.5*(-atan((Y21-Cy)/Hy)+atan((Y11-Cy)/Hy)));

    QVector<double> temp;
    temp.append(Rx1);
    temp.append(Ry1);
    return temp;

}
//Расчет радиусов по пачке экстремумов и параметров
QVector<double> AutoFindWizard::calcDiemeter2(QVector<double> dots, int ila,int iNx,int iNy,int iHx,int iHy,int iCx,int iCy){
    QVector<double> x;
    double la = laV[ila];
    double Nx = NxV[iNx];
    double Ny = NyV[iNy];
    double Hx = HxV[iHx];
    double Hy = HyV[iHy];
    double Cx = CxV[iCx];
    double Cy = CyV[iCy];

    double Front1 = dots.at(0);
    double Spad1 = dots.at(1);
    double Front2 = dots.at(2);
    double Spad2 = dots.at(3);

    //для данных с экселя
    double  X11 = -(-Front1+ Nx)*res+Cx;
    double  X21 = -(-Spad1+ Nx)*res+Cx;
    double  Y11 = -(-Front2+Ny)*res+Cy;
    double  Y21 = -(-Spad2+Ny)*res+Cy;

    //Для реальных данных
    //    double  X11 = (-Front1+ Nx)*res+Cx;
    //    double  X21 = (-Spad1+ Nx)*res+Cx;
    //    double  Y11 = (-Front2+Ny)*res+Cy;
    //    double  Y21 = (-Spad2+Ny)*res+Cy;

    double  X01 =    Hx*tan(0.5*(atan((X21-Cx)/Hx)+atan((X11-Cx)/Hx)))+Cx ;
    double  Y01 =    Hy*tan(0.5*(atan((Y21-Cy)/Hy)+atan((Y11-Cy)/Hy)))+Cy ;

    double  Ex01 =(Cx*Hy*Y01 + Hx*Hy*X01 - Hy*X01*Y01)/(Hx*Hy - Cx*Cy - X01*Y01 + Cy*X01 + Cx*Y01);
    double  Ey01 =(Hx*Cy*X01 + Hx*Hy*Y01 - Hx*X01*Y01)/(Hx*Hy - Cx*Cy - X01*Y01 + Cy*X01 + Cx*Y01);

    //Для данных с эксельки
    double Rx1 = sqrt((Ex01-Cx)*(Ex01-Cx)+(Hx-Ey01)*(Hx-Ey01))*sin(0.5*(atan((X21-Cx)/Hx)-atan((X11-Cx)/Hx)));
    double Ry1 = sqrt((Ey01-Cy)*(Ey01-Cy)+(Hy-Ex01)*(Hy-Ex01))*sin(0.5*(atan((Y21-Cy)/Hy)-atan((Y11-Cy)/Hy)));

    //Для реальных данных
    //double Rx1 = sqrt((Ex01-Cx)*(Ex01-Cx)+(Hx-Ey01)*(Hx-Ey01))*sin(0.5*(-atan((X21-Cx)/Hx)+atan((X11-Cx)/Hx)));
    //double Ry1 = sqrt((Ey01-Cy)*(Ey01-Cy)+(Hy-Ex01)*(Hy-Ex01))*sin(0.5*(-atan((Y21-Cy)/Hy)+atan((Y11-Cy)/Hy)));

    QVector<double> temp;
    temp.append(Rx1);
    temp.append(Ry1);
    return temp;

}
//Расчет СКВ ошибки определения радиусов.
double AutoFindWizard::calcErrDiemeter(int ila,int iNx,int iNy,int iHx,int iHy,int iCx,int iCy)
{
    double err0,err1,err2,err3,err4,err5,err6,err7,err8,err9,err10,err11,err12,err13,err14,err15,err16,err17;

    QVector<double> temp;
    //temp = calcDiemeter(allExtremums.at(0),ila,iNx,iNy,iHx,iHy,iCx,iCy);
    temp = calcDiemeter2(allFronts.at(0),ila,iNx,iNy,iHx,iHy,iCx,iCy);
    err0 = etalonMkm - temp.at(0);
    err1 = etalonMkm - temp.at(1);

    //temp = calcDiemeter(allExtremums.at(1),ila,iNx,iNy,iHx,iHy,iCx,iCy);
    temp = calcDiemeter2(allFronts.at(1),ila,iNx,iNy,iHx,iHy,iCx,iCy);
    err2 = etalonMkm - temp.at(0);
    err3 = etalonMkm - temp.at(1);

    //temp = calcDiemeter(allExtremums.at(2),ila,iNx,iNy,iHx,iHy,iCx,iCy);
    temp = calcDiemeter2(allFronts.at(2),ila,iNx,iNy,iHx,iHy,iCx,iCy);
    err4 = etalonMkm - temp.at(0);
    err5 = etalonMkm - temp.at(1);

    //temp = calcDiemeter(allExtremums.at(3),ila,iNx,iNy,iHx,iHy,iCx,iCy);
    temp = calcDiemeter2(allFronts.at(3),ila,iNx,iNy,iHx,iHy,iCx,iCy);
    err6 = etalonMkm - temp.at(0);
    err7 = etalonMkm - temp.at(1);

    //temp = calcDiemeter(allExtremums.at(4),ila,iNx,iNy,iHx,iHy,iCx,iCy);
    temp = calcDiemeter2(allFronts.at(4),ila,iNx,iNy,iHx,iHy,iCx,iCy);
    err8 = etalonMkm - temp.at(0);
    err9 = etalonMkm - temp.at(1);

    //temp = calcDiemeter(allExtremums.at(5),ila,iNx,iNy,iHx,iHy,iCx,iCy);
    temp = calcDiemeter2(allFronts.at(5),ila,iNx,iNy,iHx,iHy,iCx,iCy);
    err10 = etalonMkm - temp.at(0);
    err11= etalonMkm - temp.at(1);

    //temp = calcDiemeter(allExtremums.at(6),ila,iNx,iNy,iHx,iHy,iCx,iCy);
    temp = calcDiemeter2(allFronts.at(6),ila,iNx,iNy,iHx,iHy,iCx,iCy);
    err12 = etalonMkm - temp.at(0);
    err13 = etalonMkm - temp.at(1);

    //temp = calcDiemeter(allExtremums.at(7),ila,iNx,iNy,iHx,iHy,iCx,iCy);
    temp = calcDiemeter2(allFronts.at(7),ila,iNx,iNy,iHx,iHy,iCx,iCy);
    err14 = etalonMkm - temp.at(0);
    err15 = etalonMkm - temp.at(1);

    //temp = calcDiemeter(allExtremums.at(8),ila,iNx,iNy,iHx,iHy,iCx,iCy);
    temp = calcDiemeter2(allFronts.at(8),ila,iNx,iNy,iHx,iHy,iCx,iCy);
    err16 = etalonMkm - temp.at(0);
    err17 = etalonMkm - temp.at(1);

    double res= sqrt((err0*err0 + err1*err1 +err2*err2 +err3*err3 +err4*err4 +err5*err5 +err6*err6 +err7*err7 +err8*err8 +err9*err9 + err10*err10 +err11*err11 +err12*err12 +err13*err13 +err14*err14 +err15*err15 +err16*err16 +err17*err17)/18);
    return res;

}

//Принять настройки
void AutoFindWizard::on_pushButton_2_clicked()
{
    QVector<double> par {bestla,bestNx,bestNy,bestHx,bestHy,bestCx,bestCy};
    emit saveBestParameters(par);
    ui->laLabel->setText(QString::number(bestla,'g',10));
    ui->NxLabel->setText(QString::number(bestNx,'g',10));
    ui->NyLabel->setText(QString::number(bestNy,'g',10));
    ui->HxLabel->setText(QString::number(bestHx,'g',10));
    ui->HyLabel->setText(QString::number(bestHy,'g',10));
    ui->CxLabel->setText(QString::number(bestCx,'g',10));
    ui->CyLabel->setText(QString::number(bestCy,'g',10));
}


void AutoFindWizard::on_startFind_clicked(bool checked){
    if(dataCatched){
        if(checked){
             stopPressed=false;
             ui->startFind->setText("Остановить");
             autoFindAlg();
        }
        else{
           stopPressed = true;
           ui->startFind->setText("Подобрать");
        }

    }
    else
        QMessageBox::warning(this, "Внимание!", "Подбор параметров невозможнен, так как не все точки были захвачены",QMessageBox::Ok);
}

