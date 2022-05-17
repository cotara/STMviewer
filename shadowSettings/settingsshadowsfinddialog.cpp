#include "settingsshadowsfinddialog.h"
#include "ui_settingsshadowsfinddialog.h"
#include <QMessageBox>
SettingsShadowsFindDialog::SettingsShadowsFindDialog(QWidget *parent) :
    QDialog(parent), ui(new Ui::SettingsShadowsFindDialog){
    setObjectName("settingsshadowsfinddialog");
    ui->setupUi(this);

    ui->buttonBox->button(QDialogButtonBox::Ok)->setText("Сохранить");
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText("Не сохранять");

    geomParams.resize(6);

    ui->pushButton->setVisible(false);
    ui->pushButton_3->setToolTip("Восставновить параметры по умолчанию");
    ui->buttonBox->setToolTip("Сохранить настройки в файл");
    ui->buttonBox->button(QDialogButtonBox::Ok)->setToolTip("Сохранить параметры в файл");
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setToolTip("Закрыть, не сохраняя параметры в файл");

    wizard = new AutoFindWizard(this,geomParams);
    connect(wizard,&AutoFindWizard::saveBestParameters,this,&SettingsShadowsFindDialog::updateSettingsStructSlot);
    connect(ui->NxSpinBox,QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double i){ geomParams[0] = i; emit settingsChanged();});
    connect(ui->NySpinBox,QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double i){ geomParams[1] = i; emit settingsChanged();});
    connect(ui->HxSpinBox,QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double i){ geomParams[2] = i; emit settingsChanged();});
    connect(ui->HySpinBox,QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double i){ geomParams[3] = i; emit settingsChanged();});
    connect(ui->CxSpinBox,QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double i){ geomParams[4] = i; emit settingsChanged();});
    connect(ui->CySpinBox,QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double i){ geomParams[5] = i; emit settingsChanged();});

    connect(wizard,&AutoFindWizard::sendBestParameters,this,&SettingsShadowsFindDialog::sendSettingsToMK);
    connect(ui->sendToMKButton,&QPushButton::clicked,this,&SettingsShadowsFindDialog::sendSettingsToMK);

    keyF5 = new QShortcut(this);   // Инициализируем объект
    keyF5->setKey(Qt::Key_F5);    // Устанавливаем код клавиши
    // цепляем обработчик нажатия клавиши
    connect(keyF5, SIGNAL(activated()), this, SLOT(slotShortcutF5()));

}

SettingsShadowsFindDialog::~SettingsShadowsFindDialog(){
    delete ui;
}

QVector<double> &SettingsShadowsFindDialog::getShadowFindSettings(){
    return geomParams;
}

//ЗАПОЛНЯЕМ ПОЛЯ ИЗ СТРУКТУРЫ
void SettingsShadowsFindDialog::fillFields()
{
    ui->NxSpinBox->setValue(geomParams.at(0));
    ui->NySpinBox->setValue(geomParams.at(1));
    ui->HxSpinBox->setValue(geomParams.at(2));
    ui->HySpinBox->setValue(geomParams.at(3));
    ui->CxSpinBox->setValue(geomParams.at(4));
    ui->CySpinBox->setValue(geomParams.at(5));
}

void SettingsShadowsFindDialog::fillStruct(){
    geomParams[0] = ui->NxSpinBox->text().toDouble();
    geomParams[1] = ui->NxSpinBox->text().toDouble();
    geomParams[2] = ui->NxSpinBox->text().toDouble();
    geomParams[3] = ui->NxSpinBox->text().toDouble();
    geomParams[4] = ui->NxSpinBox->text().toDouble();
    geomParams[5] = ui->NxSpinBox->text().toDouble();
}

//Записывает labels (то, что сейчас в МК)
void SettingsShadowsFindDialog::filLabels(QVector<double> &par){
    ui->NxMKLabel->setText(QString::number(par.at(0)));
    ui->NyMKLabel->setText(QString::number(par.at(1)));
    ui->HxMKLabel->setText(QString::number(par.at(2)));
    ui->HyMKLabel->setText(QString::number(par.at(3)));
    ui->CxMKLabel->setText(QString::number(par.at(4)));
    ui->CyMKLabel->setText(QString::number(par.at(5)));
}


//Слот, обновляющий структуру
void SettingsShadowsFindDialog::updateSettingsStructSlot(const QVector<double> &par){
    for (int k=0;k<geomParams.size();k++)
        geomParams[k] = par.at(k);                    //Заполняем лист с настройками
    fillFields();                                      //Обновили и поля
}


//Нажали ОК
void SettingsShadowsFindDialog::on_buttonBox_accepted(){
      fillStruct();                             //Записали в структуру всё, что навводили в поля
}

//Нажали ОТМЕНА
void SettingsShadowsFindDialog::on_buttonBox_rejected(){
    fillFields();                               //Вернули в поля значения из структуры
}

//Кнопка по умолчанию
void SettingsShadowsFindDialog::on_pushButton_3_clicked(){
    switch (ldmModel){                          //Заполняем стуктуру и поля настройками по умолчанию
        case 20: updateSettingsStructSlot(ldm20Params);break;
        case 50: updateSettingsStructSlot(ldm50Params);break;
    }
}

//Нажали ПОДОБРАТЬ
void SettingsShadowsFindDialog::on_pushButton_clicked(){
    wizard->init(geomParams);
    wizard->show();
}
//Секретная клавиша, открывающая автоподбор
void SettingsShadowsFindDialog::slotShortcutF5(){
    on_pushButton_clicked();
}

