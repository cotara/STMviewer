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
    void accept();

private slots:
    void on_AutoFindWizard_currentIdChanged(int id);


    void on_pushButton_clicked();

private:
    QVBoxLayout *layout1;
    Ui::AutoFindWizard *ui;
    QVector<double> m_params;
    QVector<QVector<double>> allExtremums;
    void autoFindAlg();
    double la,L,res,Nx,Ny,Hx,Hy,Cx,Cy, p1=2.51087470;
    double initla,initL,initres,initNx,initNy,initHx,initHy,initCx,initCy;
    double dla,dL,dres,dNx,dNy,dHx,dHy,dCx,dCy;
    double bestla,bestL,bestres,bestNx,bestNy,bestHx,bestHy,bestCx,bestCy;
    double calcDiemeter(QVector<double> dots);
    double calcErrDiemeter();
    uint64_t  steps,currentStep=0;
signals:
    void giveMeExtremums();
};

#endif // AUTOFINDWIZARD_H
