#ifndef PLISSETTINGS_H
#define PLISSETTINGS_H


#include <QObject>
#include <QVBoxLayout>
#include <QLabel>
#include <QSpinBox>
#include <QPushButton>
#include <QGroupBox>

class PlisSettings: public QGroupBox
{
    Q_OBJECT

public:
    PlisSettings(QWidget *parent);
    QVBoxLayout *layout, *lazer1SettingLayout, *lazer2SettingLayout,*borderLeftLayout,*borderRightLayout,*compCH1Layout,*compCH2Layout;
    QHBoxLayout *lazerLayout, *borderLayout;
    QLabel *lazer1Label, *lazer2Label, *borderLeftLabel, *borderRightLabel,*compCH1Label,*compCH2Label;;
    QSpinBox *lazer1Spinbox, *lazer2Spinbox, *borderLeftSpinbox, *borderRightSpinbox,*compCH1Spinbox,*compCH2Spinbox;
    QPushButton *saveButton;

signals:
    void lazer1Send(int i);
    void lazer2Send(int i);
    void saveSend();
    void sendBorderLeft(int i);
    void sendBorderRight(int i);
    void sendCompCH1(int i);
    void sendCompCH2(int i);

};

#endif // PLISSETTINGS_H
