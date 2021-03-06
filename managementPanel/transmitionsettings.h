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
    QCheckBox *ch1CheckBox, *ch2CheckBox, *ch3CheckBox, *ch4CheckBox,*ch2InCheckBox, *ch4InCheckBox;
    QSpinBox *packetSizeSpinbox;
    QPushButton *getButton;
    QSignalMapper *signalMapper;
    QLabel *packetSizeLabel;

signals:
    void setPacketSize(int);
    void chChooseChanged(int);
    void getButtonClicked(bool);
    void autoGetCheckBoxChanged(int);
};

#endif // TRANSMITIONSETTINGS_H
