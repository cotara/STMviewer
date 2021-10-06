#ifndef AUTOFINDWIZARD_H
#define AUTOFINDWIZARD_H

#include <QWizard>
#include "catchdatadialog.h"
#include <QVBoxLayout>
#include <QTimer>
namespace Ui {
class AutoFindWizard;
}

class AutoFindWizard : public QWizard
{
    Q_OBJECT

public:
    explicit AutoFindWizard(QWidget *parent = nullptr,QVector<double> params = QVector<double>{0});
    ~AutoFindWizard();
    catchDataDialog *catchData;
    void setExtremums(QVector<QVector<double>> &extr);
    void accept() override;
    void init(QVector<double> params);

private slots:
    void on_AutoFindWizard_currentIdChanged(int id);


    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    QVBoxLayout *layout1;
    Ui::AutoFindWizard *ui;
    QVector<double> m_params;
    QVector<QVector<double>> allExtremums;
    void autoFindAlg();
    double res=4, p1=2.51087470;
    double initla,initNx,initNy,initHx,initHy,initCx,initCy;
    double dla,dNx,dNy,dHx,dHy,dCx,dCy;
    double bestla,bestNx,bestNy,bestHx,bestHy,bestCx,bestCy;
    QVector<double> calcDiemeter(QVector<double> dots, int ila,int iNx,int iNy,int iHx,int iHy,int iCx,int iCy);
    double calcErrDiemeter(int ila,int iNx,int iNy,int iHx,int iHy,int iCx,int iCy);
    uint64_t  steps,currentStep=0;
    int etalonMkm = 0;
    QVector <double> laV,NxV,NyV,HxV,HyV,CxV,CyV;
    //int ila=0,iNx=0,iNy=0,iHx=0,iHy=0,iCx=0,iCy=0;
signals:
    void giveMeExtremums();
    void setProgress(int progress);
    void saveBestParameters(QVector<double> &par);
};

#endif // AUTOFINDWIZARD_H
