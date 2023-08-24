#ifndef PLISSETTINGS_H
#define PLISSETTINGS_H


#include <QObject>
#include <QVBoxLayout>
#include <QLabel>
#include <QSpinBox>
#include <QPushButton>
#include <QGroupBox>
#include "entervaluewidget.h"
#include <QFile>
#include "asynchronbutton.h"
class PlisSettings: public QGroupBox
{
    Q_OBJECT

public:
    PlisSettings(QWidget *parent);
    QVBoxLayout *layout, *lazer1levelLayout,*lazer1averageLayout,*lazer1durationLayout, *lazer2levelLayout,*lazer2averageLayout,*lazer2durationLayout,*borderLeftLayout,*borderRightLayout,*compCH1Layout,*compCH2Layout, *offsetGreenLayout,*offsetBlueLayout;
    QHBoxLayout *lazer1Layout,*lazer2Layout, *borderLayout, *offsetLayout;
    QLabel *lazer1,*lazer2;
    QLabel *lazer1levelLabel, *lazer1averageLabel,*lazer1durationLabel,*lazer2levelLabel, *lazer2averageLabel,*lazer2durationLabel, *borderLeftLabel, *borderRightLabel,*compCH1Label,*compCH2Label;;
    QLabel *lazer1averageNum,*lazer1durationNum, *lazer2averageNum,*lazer2durationNum;
    QLabel *offsetGreenLabel, *offsetBlueLabel;
    AsynchronButton *offsetGreenButton, *offsetBlueButton;
    AsynchronButton *lazer1Button, *lazer2Button;
    AsynchronButton *borderLeftButton, *borderRightButton, *compCH1Button, *compCH2Button;
    QPushButton *saveButton;
    EnterValueWidget *borderLeftWidget,*borderRightWidget,*compCH1Widget,*compCH2Widget;
    //const double nsTotugr = 25.0/6;

signals:
    void lazer1Send(int i);
    void lazer2Send(int i);
    void saveSend();
    void sendBorderLeft(int i);
    void sendBorderRight(int i);
    void sendCompCH1(int i);
    void sendCompCH2(int i);
    void sendGreenOffset(int i);
    void sendBlueOffset(int i);

};

#endif // PLISSETTINGS_H
