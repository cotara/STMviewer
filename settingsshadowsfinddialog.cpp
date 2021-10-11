#include "settingsshadowsfinddialog.h"
#include "ui_settingsshadowsfinddialog.h"
#include <QMessageBox>
SettingsShadowsFindDialog::SettingsShadowsFindDialog(QWidget *parent) :
    QDialog(parent), ui(new Ui::SettingsShadowsFindDialog){
    setObjectName("settingsshadowsfinddialog");
    ui->setupUi(this);

    ui->buttonBox->button(QDialogButtonBox::Ok)->setText("Сохранить");
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText("Не сохранять");

    file = new QFile();
    file->setFileName(filename);
    paramsDouble.resize(7);
    updateSettingsStruct();     //Обновили структуру из файла

    wizard = new AutoFindWizard(this,paramsDouble);
    connect(wizard,&AutoFindWizard::saveBestParameters,this,&SettingsShadowsFindDialog::updateSettingsStructSlot);
    connect(ui->laSpinBox,QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double i){ paramsDouble[0] = i; emit settingsChanged();});
    connect(ui->NxSpinBox,QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double i){ paramsDouble[1] = i; emit settingsChanged();});
    connect(ui->NySpinBox,QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double i){ paramsDouble[2] = i; emit settingsChanged();});
    connect(ui->HxSpinBox,QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double i){ paramsDouble[3] = i; emit settingsChanged();});
    connect(ui->HySpinBox,QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double i){ paramsDouble[4] = i; emit settingsChanged();});
    connect(ui->CxSpinBox,QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double i){ paramsDouble[5] = i; emit settingsChanged();});
    connect(ui->CySpinBox,QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double i){ paramsDouble[6] = i; emit settingsChanged();});

    connect(wizard,&AutoFindWizard::sendBestParameters,this,&SettingsShadowsFindDialog::sendSettingsToMK);
}

SettingsShadowsFindDialog::~SettingsShadowsFindDialog(){
    delete ui;
}

QVector<double> &SettingsShadowsFindDialog::getShadowFindSettings(){
    return paramsDouble;
}
//Прочитать настройки из файла
QByteArray SettingsShadowsFindDialog::readParamsFromFile()
{
    QByteArray tempBuf;
    if(!file->open(QIODevice::ReadWrite)){
        QMessageBox::warning(this, "Внимание!", "Файл для чтения параметров расчета диаметра не может быть открыт",QMessageBox::Ok);
        return tempBuf;
    }
    tempBuf = file->readAll();
    file->close();
    return tempBuf;
}

//ЗАПОЛНЯЕМ ПОЛЯ ИЗ СТРУКТУРЫ
void SettingsShadowsFindDialog::fillFileads()
{
    ui->laSpinBox->setValue(paramsDouble.at(0));
    ui->NxSpinBox->setValue(paramsDouble.at(1));
    ui->NySpinBox->setValue(paramsDouble.at(2));
    ui->HxSpinBox->setValue(paramsDouble.at(3));
    ui->HySpinBox->setValue(paramsDouble.at(4));
    ui->CxSpinBox->setValue(paramsDouble.at(5));
    ui->CySpinBox->setValue(paramsDouble.at(6));
}

//ЗАПИСЫВАЕМ В ФАЙЛ ИЗ СТРУКТУРЫ
void SettingsShadowsFindDialog::writeToFile()
{
    QString tempToWrite;
    tempToWrite =  "la=" + QString::number(paramsDouble.at(0),'g',10)+ '\n'+        //Здесь обрезает до 6 знаков после запятой
                    "Nx=" + QString::number(paramsDouble.at(1),'g',10) + '\n'+
                    "Ny=" + QString::number(paramsDouble.at(2),'g',10) + '\n'+
                    "Hx=" + QString::number(paramsDouble.at(3),'g',10) + '\n'+
                    "Hy=" + QString::number(paramsDouble.at(4),'g',10) + '\n'+
                    "Cx=" + QString::number(paramsDouble.at(5),'g',10) + '\n'+
                    "Cy=" + QString::number(paramsDouble.at(6),'g',10);
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


//ОБНОВЛЯЕМ СТРУКТУРУ ИЗ ФАЙЛА (ПРИ ЗАПИСИ НАСТРОЕК ПО УМОЛЧАНИЮ или при отказе от сохранения настроек)
void SettingsShadowsFindDialog::updateSettingsStruct()
{
    if(file->isOpen())
        file->close();

    paramsDouble.fill(0);

    QByteArray tempBuf = readParamsFromFile();
    QList<QByteArray> list_params=tempBuf.split('\n');


    if(list_params.size()!=paramsDouble.size()){                   //Если файл пустой, то заполним его дефолтными настройками
        QMessageBox::warning(this, "Внимание!", "Файл с параметрами расчета диаметра был поврежден. Возвращены параметры по умолчанию",QMessageBox::Ok);
        defaultToFile();
        tempBuf = readParamsFromFile();
        list_params=tempBuf.split('\n');
    }

    for (int k=0;k<list_params.count();k++){
        QList<QByteArray> param = list_params.at(k).split('=');
        paramsDouble[k] = param.at(1).toDouble();                    //Заполняем лист с настройками
    }
    fillFileads();
}

//Слот, обновляющий структуру
void SettingsShadowsFindDialog::updateSettingsStructSlot(QVector<double> &par)
{
    for (int k=0;k<paramsDouble.size();k++)
        paramsDouble[k] = par.at(k);                    //Заполняем лист с настройками
    fillFileads();                                      //Обновили и поля
}

//Восставновить настройки по умолчанию
void SettingsShadowsFindDialog::defaultToFile(){

    updateSettingsStructSlot(defaultSettings);
}

//Нажали ОК
void SettingsShadowsFindDialog::on_buttonBox_accepted(){
    writeToFile();              //Записали из полей в файл
}

//Нажали ОТМЕНА
void SettingsShadowsFindDialog::on_buttonBox_rejected(){
    updateSettingsStruct();      //Заполнили поля из файла
}

//Кнопка по умолчанию
void SettingsShadowsFindDialog::on_pushButton_3_clicked(){
    defaultToFile();
}

//Нажали ПОДОБРАТЬ
void SettingsShadowsFindDialog::on_pushButton_clicked(){
    wizard->init(paramsDouble);
    wizard->show();
}
