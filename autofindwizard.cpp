#include "autofindwizard.h"
#include "ui_autofindwizard.h"
#include <cmath>

AutoFindWizard::AutoFindWizard(QWidget *parent, QVector<double> params) :
    QWizard(parent),
    ui(new Ui::AutoFindWizard)
{
    ui->setupUi(this);
    layout1 = new QVBoxLayout();
    catchData = new catchDataDialog(this);
    ui->wizardPage1->setLayout(layout1);
    layout1->addWidget(catchData);
    m_params=params;

    if(params.size() == 9){
        ui->laLabel->setText(QString::number(params.at(0),'g',10));
        ui->LLabel->setText(QString::number(params.at(1),'g',10));
        ui->resLabel->setText(QString::number(params.at(2),'g',10));
        ui->NxLabel->setText(QString::number(params.at(3),'g',10));
        ui->NyLabel->setText(QString::number(params.at(4),'g',10));
        ui->HxLabel->setText(QString::number(params.at(5),'g',10));
        ui->HyLabel->setText(QString::number(params.at(6),'g',10));
        ui->CxLabel->setText(QString::number(params.at(7),'g',10));
        ui->CyLabel->setText(QString::number(params.at(8),'g',10));

        la = m_params.at(0);
        L = m_params.at(1);
        res = m_params.at(2);
        Nx = m_params.at(3);
        Ny = m_params.at(4);
        Hx = m_params.at(5);
        Hy = m_params.at(6);
        Cx = m_params.at(7);
        Cy = m_params.at(8);
    }

}

AutoFindWizard::~AutoFindWizard()
{
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
    hide();
}

//Алгоритм подбора параметров
void AutoFindWizard::autoFindAlg()
{

    if(allExtremums.size()!=9)
        return;
    //Начинаем поиск только если в наличии 9 валидных точек

    initla =   m_params.at(0);
    initL =  m_params.at(1);
    initres = m_params.at(2);
    initNx = m_params.at(3);
    initNy = m_params.at(4);
    initHx = m_params.at(5);
    initHy = m_params.at(6);
    initCx = m_params.at(7);
    initCy = m_params.at(8);

    //Устанавливем начальные значения для пребора
    initla *=   (100-ui->laRange->value())/100.0;
    initL *=  (100-ui->LRange->value())/100.0;
    initres *= (100-ui->resRange->value())/100.0;
    initNx *= (100-ui->NxRange->value())/100.0;
    initNy *= (100-ui->NyRange->value())/100.0;
    initHx *= (100-ui->HxRange->value())/100.0;
    initHy *= (100-ui->HyRange->value())/100.0;
    initCx *= (100-ui->CxRange->value())/100.0;
    initCy *= (100-ui->CyRange->value())/100.0;

    //Рассчитываем шаг перебора (1%)
    dla = la/100;
    dL = L/100;
    dres = res/100;
    dNx = Nx/100;
    dNy = Ny/100;
    dHx = Hx/100;
    dHy = Hy/100;
    dCx = Cx/100;
    dCy = Cy/100;
    double err,bestErr=10000;
    steps = (ui->laRange->value()*2+1)* (ui->LRange->value()*2+1)*(ui->resRange->value()*2+1)*
            (ui->NxRange->value()*2+1)* (ui->NyRange->value()*2+1)*(ui->HxRange->value()*2+1)*
            (ui->HyRange->value()*2+1)* (ui->CxRange->value()*2+1)*(ui->CxRange->value()*2+1);
    steps/=100;
    currentStep = 0;
    uint64_t progress = 0;


    la=initla;
    for (int ila = 0;ila<(ui->laRange->value())*2+1;ila++){
        L=initL;
        for (int iL = 0;iL<(ui->LRange->value())*2+1;iL++){
            res=initres;
            for (int ires = 0;ires<(ui->resRange->value())*2+1;ires++){
                Nx = initNx;
                for (int iNx = 0;iNx<(ui->NxRange->value())*2+1;iNx++){
                    Ny = initNy;
                    for (int iNy = 0;iNy<(ui->NyRange->value())*2+1;iNy++){
                        Hx = initHx;
                        for (int iHx = 0;iHx<(ui->HxRange->value())*2+1;iHx++){
                            Hy = initHy;
                            for (int iHy = 0;iHy<(ui->HyRange->value())*2+1;iHy++){
                                Cx = initCx;
                                for (int iCx = 0;iCx<(ui->CxRange->value())*2+1;iCx++){
                                   Cy = initCy;
                                    for (int iCy = 0;iCy<(ui->CyRange->value())*2+1;iCy++){
                                        err = calcErrDiemeter();
                                        ui->progressBar->setValue(currentStep++/steps);
                                        if(err<bestErr){
                                            bestErr = err;
                                            bestla = la;
                                            bestL = L;
                                            bestres = res;
                                            bestNx = Nx;
                                            bestNy = Ny;
                                            bestHx = Hx;
                                            bestHy = Hy;
                                            bestCx = Cx;
                                            bestCy = Cy;
                                        }
                                        Cy+=dCy;
                                    }
                                    Cx+=dCx;
                                }
                                Hy+=dHy;
                            }
                            Hx+=dHx;
                        }

                        Ny+=dNy;
                        QApplication::processEvents();
                    }
                    Nx+=dNx;
                }
                res+=dres;
            }
            L+=dL;
        }
        la+=dla;
    }

    ui->laSpinBox_2->setValue(bestla);
    ui->LSpinBox_2->setValue(bestL);
    ui->resSpinBox_2->setValue(bestres);
    ui->NxSpinBox_2->setValue(bestNx);
    ui->NySpinBox_2->setValue(bestNy);
    ui->HxSpinBox_2->setValue(bestHx);
    ui->HySpinBox_2->setValue(bestHy);
    ui->CxSpinBox_2->setValue(bestCx);
    ui->CySpinBox_2->setValue(bestCy);
    ui->resultDiameter->setText(QString::number(ui->etalonDiameterSpinBox->value()+bestErr/1000,'g',10));

}


double AutoFindWizard::calcDiemeter(QVector<double> dots){
    QVector<double> x;
    double delta1 = dots.at(1)-dots.at(0);
    double delta2 = dots.at(3)-dots.at(2);
    double delta3 = dots.at(5)-dots.at(4);
    double delta4 = dots.at(7)-dots.at(6);

    double y1=la*L*L*p1/(4*delta1*delta1*res*res + la*L*p1);
    double y2=la*L*L*p1/(4*delta2*delta2*res*res + la*L*p1);
    double y3=la*L*L*p1/(4*delta3*delta3*res*res + la*L*p1);
    double y4=la*L*L*p1/(4*delta4*delta4*res*res + la*L*p1);

    double x01=dots.at(1)+sqrt(la*L*(L-y1)*1.5/(2*y1))/res; //Левая тень
    double x02=dots.at(2)-sqrt(la*L*(L-y2)*1.5/(2*y2))/res; //Правая тень

    double x03=dots.at(5)+sqrt(la*L*(L-y3)*1.5/(2*y3))/res;
    double x04=dots.at(6)-sqrt(la*L*(L-y4)*1.5/(2*y4))/res;


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

double AutoFindWizard::calcErrDiemeter()
{
    double err0,err1,err2,err3,err4,err5,err6,err7,err8;

    err0 = ui->etalonDiameterSpinBox->value()*1000 - calcDiemeter(allExtremums.at(0));
    err1 = ui->etalonDiameterSpinBox->value()*1000 - calcDiemeter(allExtremums.at(1));
    err2 = ui->etalonDiameterSpinBox->value()*1000 - calcDiemeter(allExtremums.at(2));
    err3 = ui->etalonDiameterSpinBox->value()*1000 - calcDiemeter(allExtremums.at(3));
    err4 = ui->etalonDiameterSpinBox->value()*1000 - calcDiemeter(allExtremums.at(4));
    err5 = ui->etalonDiameterSpinBox->value()*1000 - calcDiemeter(allExtremums.at(5));
    err6 = ui->etalonDiameterSpinBox->value()*1000 - calcDiemeter(allExtremums.at(6));
    err7 = ui->etalonDiameterSpinBox->value()*1000 - calcDiemeter(allExtremums.at(7));
    err8 = ui->etalonDiameterSpinBox->value()*1000 - calcDiemeter(allExtremums.at(8));

    double res= sqrt((err0*err0 + err1*err1 +err2*err2 +err3*err3 +err4*err4 +err5*err5 +err6*err6 +err7*err7 +err8*err8)/9);
    return res;

}




void AutoFindWizard::on_pushButton_clicked()
{
    autoFindAlg();
}
