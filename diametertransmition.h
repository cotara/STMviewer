#ifndef DIAMETERTRANSMITION_H
#define DIAMETERTRANSMITION_H

#include <QGroupBox>
#include <QObject>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QSpinBox>
#include <QLabel>


class DiameterTransmition : public QGroupBox
{
    Q_OBJECT
public:
    DiameterTransmition(QWidget *parent);


    QCheckBox *gettingDiameterCheckbox, *medianFilterCheckbox;
    QSpinBox *windowSizeSpinbox,*averageSpinbox,*reqFreqSpinbox;

private:
    QVBoxLayout *layout;
    QHBoxLayout *windowSizeLayout, *averageLayout, *reqFreqLayout;
    QLabel *windowSizeLabel, *averageLabel, *reqFreqLabel;

signals:
    void getDiameterChanged(int state);
    void reqFreqValueChanged(int value);
};

#endif // DIAMETERTRANSMITION_H
