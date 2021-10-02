#include "settingsshadowsfinddialog.h"
#include "ui_settingsshadowsfinddialog.h"
#include <QMessageBox>
SettingsShadowsFindDialog::SettingsShadowsFindDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsShadowsFindDialog)
{
    setObjectName("settingsshadowsfinddialog");
    ui->setupUi(this);
    file = new QFile();

    file->setFileName(filename);
    updateSettingsStruct();     //Обновили настройки из файла
    fillFileads();              //Заполнили поля

    wizard = new AutoFindWizard(this,paramsDouble);
}

SettingsShadowsFindDialog::~SettingsShadowsFindDialog(){
    delete ui;
}

QVector<double> &SettingsShadowsFindDialog::getShadowFindSettings(){
    return paramsDouble;
}

//ЗАПОЛНЯЕМ ПОЛЯ ИЗ СТРУКТУРЫ
void SettingsShadowsFindDialog::fillFileads()
{
    ui->laSpinBox->setValue(paramsDouble.at(0));
    ui->LSpinBox->setValue(paramsDouble.at(1));
    ui->resSpinBox->setValue(paramsDouble.at(2));
    ui->NxSpinBox->setValue(paramsDouble.at(3));
    ui->NySpinBox->setValue(paramsDouble.at(4));
    ui->HxSpinBox->setValue(paramsDouble.at(5));
    ui->HySpinBox->setValue(paramsDouble.at(6));
    ui->CxSpinBox->setValue(paramsDouble.at(7));
    ui->CySpinBox->setValue(paramsDouble.at(8));
}
//ЗАПИСЫВАЕМ В ФАЙЛ ИЗ ПОЛЕЙ
void SettingsShadowsFindDialog::writeToFile()
{
    QString tempToWrite;
    tempToWrite =  "la=" +QString::number(ui->laSpinBox->value(),'g',10)+ '\n'+        //Здесь обрезает до 6 знаков после запятой
                   "L=" + QString::number(ui->LSpinBox->value(),'g',10) + '\n'+
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

//ОБНОВЛЯЕМ СТРУКТУРУ ИЗ ФАЙЛА
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
    tempToWrite =  "la=" +QString::number(0.905,'g',10)+ '\n'+        //Здесь обрезает до 6 знаков после запятой
                   "L=" + QString::number(207400,'g',10) + '\n'+
                    "res=" + QString::number(4,'g',10) + '\n'+
                    "Nx=" + QString::number(5320,'g',10) + '\n'+
                    "Ny=" + QString::number(5320,'g',10) + '\n'+
                    "Hx=" + QString::number(207400,'g',10) + '\n'+
                    "Hy=" + QString::number(207400,'g',10) + '\n'+
                    "Cx=" + QString::number(73400,'g',10) + '\n'+
                    "Cy=" + QString::number(73400,'g',10);
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

    updateSettingsStruct();
    fillFileads();
}
void SettingsShadowsFindDialog::on_buttonBox_accepted(){
    writeToFile();              //Записали из полей в файл
    updateSettingsStruct();     //Обновили переменную из файла
    emit settingsChanged();
}

void SettingsShadowsFindDialog::on_buttonBox_rejected(){
    fillFileads();      //Заполнили поля из переменной
}

void SettingsShadowsFindDialog::on_pushButton_3_clicked()
{
    defaultToFile();
}

void SettingsShadowsFindDialog::on_pushButton_clicked()
{
    wizard->show();
}
