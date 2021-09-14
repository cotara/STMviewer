#include "settingsshadowsfinddialog.h"
#include "ui_settingsshadowsfinddialog.h"
#include <QMessageBox>
SettingsShadowsFindDialog::SettingsShadowsFindDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsShadowsFindDialog)
{
    ui->setupUi(this);
    file = new QFile();

    file->setFileName(filename);
    updateSettingsStruct();     //Обновили настройки из файла
    fillFileads();            //Заполнили поля
}

SettingsShadowsFindDialog::~SettingsShadowsFindDialog(){
    delete ui;
}

QList<double> &SettingsShadowsFindDialog::getShadowFindSettings(){
    return paramsDouble;
}

void SettingsShadowsFindDialog::fillFileads()
{
    ui->laSpinBox->setValue(paramsDouble.at(0));
    ui->LSpinBox->setValue(paramsDouble.at(1));
    ui->p1SpinBox->setValue(paramsDouble.at(2));
    ui->p2SpinBox->setValue(paramsDouble.at(3));
    ui->p3SpinBox->setValue(paramsDouble.at(4));
    ui->resSpinBox->setValue(paramsDouble.at(5));
    ui->NxSpinBox->setValue(paramsDouble.at(6));
    ui->NySpinBox->setValue(paramsDouble.at(7));
    ui->HxSpinBox->setValue(paramsDouble.at(8));
    ui->HySpinBox->setValue(paramsDouble.at(9));
    ui->CxSpinBox->setValue(paramsDouble.at(10));
    ui->CySpinBox->setValue(paramsDouble.at(11));
}

void SettingsShadowsFindDialog::writeToFile()
{
    QString tempToWrite;
    tempToWrite =  "la=" +QString::number(ui->laSpinBox->value(),'g',10)+ '\n'+        //Здесь обрезает до 6 знаков после запятой
                   "L=" + QString::number(ui->LSpinBox->value(),'g',10) + '\n'+
                    "p1=" + QString::number(ui->p1SpinBox->value(),'g',10) + '\n'+
                    "p2=" + QString::number(ui->p2SpinBox->value(),'g',10) + '\n'+
                    "p3=" + QString::number(ui->p3SpinBox->value(),'g',10) + '\n'+
                    "res=" + QString::number(ui->resSpinBox->value(),'g',10) + '\n'+
                    "Nx=" + QString::number(ui->NxSpinBox->value(),'g',10) + '\n'+
                    "Ny=" + QString::number(ui->NySpinBox->value(),'g',10) + '\n'+
                    "Hx=" + QString::number(ui->HxSpinBox->value(),'g',10) + '\n'+
                    "Hy=" + QString::number(ui->HySpinBox->value(),'g',10) + '\n'+
                    "Cx=" + QString::number(ui->CxSpinBox->value(),'g',10) + '\n'+
                    "Cy=" + QString::number(ui->CySpinBox->value(),'g',10);
    if(file->isOpen())
         file->close();
    if(!file->open(QIODevice::WriteOnly)){
        QMessageBox::warning(this, "Внимание!", "Файл для записи настроек не открыт",QMessageBox::Ok);
        return;
    }

    if(file->write(tempToWrite.toUtf8())==-1){
        QMessageBox::warning(this, "Внимание!", "Запись настроек в файл не удалась",QMessageBox::Ok);
        return;
    }
    file->close();
}

void SettingsShadowsFindDialog::updateSettingsStruct()
{
    if(file->isOpen())
        file->close();

    if(!file->open(QIODevice::ReadWrite)){
        QMessageBox::warning(this, "Внимание!", "Файл для чтения настроек не может быть открыт",QMessageBox::Ok);
        return;
    }

    QByteArray tempBuf = file->readAll();
    if(tempBuf.size()==0){                   //Если файл пустой, то заполним его
        defaultToFile();
        if(!file->open(QIODevice::ReadWrite)){
            QMessageBox::warning(this, "Внимание!", "Файл для чтения настроек не может быть открыт",QMessageBox::Ok);
            return;
        }
        tempBuf = file->readAll();
    }

    paramsDouble.clear();
    QList<QByteArray> list_params=tempBuf.split('\n');
    for (int k=0;k<list_params.count();k++){
        QList<QByteArray> param = list_params.at(k).split('=');
        paramsDouble.append(param.at(1).toDouble());                    //Заполняем лист с настройками
    }
    file->close();
}



void SettingsShadowsFindDialog::defaultToFile()
{

    QString tempToWrite;
    tempToWrite =  "la=" +QString::number(0.000000905,'g',10)+ '\n'+        //Здесь обрезает до 6 знаков после запятой
                   "L=" + QString::number(0.2074,'g',10) + '\n'+
                    "p1=" + QString::number(2.51087470,'g',10) + '\n'+
                    "p2=" + QString::number(0.83484861,'g',10) + '\n'+
                    "p3=" + QString::number(0.45007122,'g',10) + '\n'+
                    "res=" + QString::number(0.000004,'g',10) + '\n'+
                    "Nx=" + QString::number(5320,'g',10) + '\n'+
                    "Ny=" + QString::number(5320,'g',10) + '\n'+
                    "Hx=" + QString::number(207.4,'g',10) + '\n'+
                    "Hy=" + QString::number(207.4,'g',10) + '\n'+
                    "Cx=" + QString::number(73.4,'g',10) + '\n'+
                    "Cy=" + QString::number(73.4,'g',10);
    if(file->isOpen())
         file->close();
    if(!file->open(QIODevice::WriteOnly)){
        QMessageBox::warning(this, "Внимание!", "Файл для записи настроек не открыт",QMessageBox::Ok);
        return;
    }

    if(file->write(tempToWrite.toUtf8())==-1){
        QMessageBox::warning(this, "Внимание!", "Запись настроек в файл не удалась",QMessageBox::Ok);
        return;
    }
    file->close();

}
void SettingsShadowsFindDialog::on_buttonBox_accepted(){
    writeToFile();              //Записали из полей в файл
    updateSettingsStruct();     //Обновили переменную из файла
}

void SettingsShadowsFindDialog::on_buttonBox_rejected(){
    fillFileads();      //Заполнили поля из переменной
}
