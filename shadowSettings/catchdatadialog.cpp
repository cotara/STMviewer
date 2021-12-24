#include "catchdatadialog.h"
#include "ui_catchdatadialog.h"
#include <QFile>
catchDataDialog::catchDataDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::catchDataDialog)
{
    ui->setupUi(this);

    connect(ui->pushButton,&QPushButton::clicked,[=]{emit buttonClicked(1);});
    connect(ui->pushButton_2,&QPushButton::clicked,[=]{emit buttonClicked(2);});
    connect(ui->pushButton_3,&QPushButton::clicked,[=]{emit buttonClicked(3);});
    connect(ui->pushButton_4,&QPushButton::clicked,[=]{emit buttonClicked(4);});
    connect(ui->pushButton_5,&QPushButton::clicked,[=]{emit buttonClicked(5);});
    connect(ui->pushButton_6,&QPushButton::clicked,[=]{emit buttonClicked(6);});
    connect(ui->pushButton_7,&QPushButton::clicked,[=]{emit buttonClicked(7);});
    connect(ui->pushButton_8,&QPushButton::clicked,[=]{emit buttonClicked(8);});
    connect(ui->pushButton_9,&QPushButton::clicked,[=]{emit buttonClicked(9);});
}

catchDataDialog::~catchDataDialog()
{
    delete ui;
}

void catchDataDialog::setButtonPushed(QVector <double> data, int i)
{
    QByteArray styleGreen,styleYellow;
    QFile file;
    QByteArray style;
    file.setFileName(":/qss/styleYelowButtons.css");//Красим кнопку желтой
    if(file.open(QFile::ReadOnly)){
          style = file.readAll();
          file.close();
    }
    QString text;
    for(int i = 0;i<data.size()-1;i+=2)
        text+=QString::number(data.at(i)) + " / " + QString::number(data.at(i+1)) + "\n";

    if(data.size() == 9){//Если получены все экстремумы
        text+=QString::number(data.last());
        data.pop_back();//Удаляем последний элемент
        if(data.at(0)<data.at(1) && data.at(1)<data.at(2) && data.at(2)<data.at(3)) //Если все экстремумы по порядку
            if(data.at(4)<data.at(5) && data.at(5)<data.at(6) && data.at(6)<data.at(7) && data.at(8)>0 ){
                file.setFileName(":/qss/styleGreenButtons.css");                          //Красим кнопку зеленой
                if(file.open(QFile::ReadOnly)){
                      style = file.readAll();
                      file.close();
                }
            }

    }
    switch(i){
    case 1:
        buttonStatus[0]=true;               //Ответ (валидный или нет) от кнопки получен
        ui->pushButton->setText(text);      //Значения экстремумов на кнопку помещены
        ui->pushButton->setStyleSheet(style);//Цвет кнопки в соответствии с валидностью данных
        emit pointCatched(data,1);
        break;
    case 2:
        buttonStatus[1]=true;
        ui->pushButton_2->setText(text);
        ui->pushButton_2->setStyleSheet(style);
        emit pointCatched(data,2);
        break;
    case 3:
        buttonStatus[2]=true;
        ui->pushButton_3->setText(text);
        ui->pushButton_3->setStyleSheet(style);
        emit pointCatched(data,3);
        break;
    case 4:
        buttonStatus[3]=true;
        ui->pushButton_4->setText(text);
        ui->pushButton_4->setStyleSheet(style);
        emit pointCatched(data,4);
        break;
    case 5:
        buttonStatus[4]=true;
        ui->pushButton_5->setText(text);
        ui->pushButton_5->setStyleSheet(style);
        emit pointCatched(data,5);
        break;
    case 6:
        buttonStatus[5]=true;
        ui->pushButton_6->setText(text);
        ui->pushButton_6->setStyleSheet(style);
         pointCatched(data,6);
        break;
    case 7:
        buttonStatus[6]=true;
        ui->pushButton_7->setText(text);
        ui->pushButton_7->setStyleSheet(style);
        emit pointCatched(data,7);
        break;
    case 8:
        buttonStatus[7]=true;
        ui->pushButton_8->setText(text);
        ui->pushButton_8->setStyleSheet(style);
        emit pointCatched(data,8);
        break;
    case 9:
        buttonStatus[8]=true;
        ui->pushButton_9->setText(text);
        ui->pushButton_9->setStyleSheet(style);
        emit pointCatched(data,9);
        break;
    }
    checkStatus();
}

void catchDataDialog::clear()
{
    QFile file;
    QByteArray style;
    file.setFileName(":/qss/styleWhiteButtons.css");//Красим кнопку желтой
    if(file.open(QFile::ReadOnly)){
          style = file.readAll();
          file.close();
    }

    ui->pushButton->setText("");
    ui->pushButton_2->setText("");
    ui->pushButton_3->setText("");
    ui->pushButton_4->setText("");
    ui->pushButton_5->setText("");
    ui->pushButton_6->setText("");
    ui->pushButton_7->setText("");
    ui->pushButton_8->setText("");
    ui->pushButton_9->setText("");

    ui->pushButton->setStyleSheet(style);
    ui->pushButton_2->setStyleSheet(style);
    ui->pushButton_3->setStyleSheet(style);
    ui->pushButton_4->setStyleSheet(style);
    ui->pushButton_5->setStyleSheet(style);
    ui->pushButton_6->setStyleSheet(style);
    ui->pushButton_7->setStyleSheet(style);
    ui->pushButton_8->setStyleSheet(style);
    ui->pushButton_9->setStyleSheet(style);

    buttonStatus.fill(false);
}

//Проверяет пришел ли ответ на все 9 кнопок и если да, то отправляет сигнал, что можно идти дальше
void catchDataDialog::checkStatus()
{
    int st=0;

    for(bool i : buttonStatus){
        if (i)
            st++;
    }

    if(st==9){
        mainStatus = true;
        emit dataCatched();
    }
    else
        mainStatus = false;
}
