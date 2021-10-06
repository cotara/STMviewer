#include "autofindwizard.h"
#include "ui_autofindwizard.h"
#include <cmath>
#include <omp.h>
#include <qdebug.h>
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
    connect(this,&AutoFindWizard::setProgress,ui->progressBar,&QProgressBar::setValue);
    connect(ui->etalonDiameterSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                [=](double i){ etalonMkm = static_cast<int>(1000*i);});

    etalonMkm = static_cast<int>(1000*ui->etalonDiameterSpinBox->value());
}

void AutoFindWizard::init(QVector<double> params)
{
    m_params=params;

    if(params.size() == 7){
        ui->laLabel->setText(QString::number(params.at(0),'g',10));
        ui->NxLabel->setText(QString::number(params.at(1),'g',10));
        ui->NyLabel->setText(QString::number(params.at(2),'g',10));
        ui->HxLabel->setText(QString::number(params.at(3),'g',10));
        ui->HyLabel->setText(QString::number(params.at(4),'g',10));
        ui->CxLabel->setText(QString::number(params.at(5),'g',10));
        ui->CyLabel->setText(QString::number(params.at(6),'g',10));
    }
    catchData->clear();
    restart();
    allExtremums.clear();
}

AutoFindWizard::~AutoFindWizard(){
    delete ui;
}

void AutoFindWizard::on_AutoFindWizard_currentIdChanged(int id){
   if(id == 1) {    //Мы перешли на страницу подбора
       emit giveMeExtremums();
   }

}
void AutoFindWizard::setExtremums(QVector<QVector<double> > &extr){
    allExtremums = extr;
}

void AutoFindWizard::accept()
{
    catchData->clear();
    QDialog::accept();
    restart();
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


    #if defined(_OPENMP)
      qDebug() << "Compiled by an OpenMP-compliant implementation.\n";
      qDebug() <<  omp_get_num_threads();
    #endif
        //omp_set_num_threads(2);
      double start;
      double end;
      start = omp_get_wtime();
int ila=0,iNx=0,iNy=0,iHx=0,iHy=0,iCx=0,iCy=0;
#pragma omp parallel  private(err,ila,iNx,iNy, iHx,iHy,iCx,iCy)
   {
    #pragma omp  for //nowait//ordered //schedule(static)
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
                            }
                        }
                    }
                    QApplication::processEvents();
                    emit setProgress(currentStep/steps);
                }
            }
        }
    }
}
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


double AutoFindWizard::calcDiemeter(QVector<double> dots, int ila,int iNx,int iNy,int iHx,int iHy,int iCx,int iCy){
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

    return Rx1+Ry1;

}

double AutoFindWizard::calcErrDiemeter(int ila,int iNx,int iNy,int iHx,int iHy,int iCx,int iCy)
{
    double err0,err1,err2,err3,err4,err5,err6,err7,err8;

    err0 = etalonMkm - calcDiemeter(allExtremums.at(0),ila,iNx,iNy,iHx,iHy,iCx,iCy);
    err1 = etalonMkm - calcDiemeter(allExtremums.at(1),ila,iNx,iNy,iHx,iHy,iCx,iCy);
    err2 = etalonMkm - calcDiemeter(allExtremums.at(2),ila,iNx,iNy,iHx,iHy,iCx,iCy);
    err3 = etalonMkm - calcDiemeter(allExtremums.at(3),ila,iNx,iNy,iHx,iHy,iCx,iCy);
    err4 = etalonMkm - calcDiemeter(allExtremums.at(4),ila,iNx,iNy,iHx,iHy,iCx,iCy);
    err5 = etalonMkm - calcDiemeter(allExtremums.at(5),ila,iNx,iNy,iHx,iHy,iCx,iCy);
    err6 = etalonMkm - calcDiemeter(allExtremums.at(6),ila,iNx,iNy,iHx,iHy,iCx,iCy);
    err7 = etalonMkm - calcDiemeter(allExtremums.at(7),ila,iNx,iNy,iHx,iHy,iCx,iCy);
    err8 = etalonMkm - calcDiemeter(allExtremums.at(8),ila,iNx,iNy,iHx,iHy,iCx,iCy);

    double res= sqrt((err0*err0 + err1*err1 +err2*err2 +err3*err3 +err4*err4 +err5*err5 +err6*err6 +err7*err7 +err8*err8)/9);
    return res;

}




void AutoFindWizard::on_pushButton_clicked()
{
    autoFindAlg();
}

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

