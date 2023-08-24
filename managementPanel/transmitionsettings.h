#ifndef TRANSMITIONSETTINGS_H
#define TRANSMITIONSETTINGS_H

#include <QObject>
#include <QGroupBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QSignalMapper>
class TransmitionSettings : public QGroupBox
{
    Q_OBJECT

public:
    TransmitionSettings(QWidget *parent);
    QVBoxLayout *layout;
    QHBoxLayout *shift1Layout,*shift2Layout;
    QCheckBox *ch1CheckBox, *ch2CheckBox, *ch3CheckBox, *ch4CheckBox,*ch2InCheckBox, *ch4InCheckBox;
    QSpinBox *packetSizeSpinbox, *shiftSpinbox, *shift2Spinbox;
    QPushButton *getButton;
    QSignalMapper *signalMapper;
    QLabel *packetSizeLabel,*shift1Label, *shift2Label;

signals:
    void setPacketSize(int);
    void chChooseChanged(int);
    void getButtonClicked(bool);
    void autoGetCheckBoxChanged(int);
};

#endif // TRANSMITIONSETTINGS_H
