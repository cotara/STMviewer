#ifndef DIAMETERTRANSMITION_H
#define DIAMETERTRANSMITION_H

#include <QGroupBox>
#include <QObject>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QSpinBox>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
class DiameterTransmition : public QGroupBox
{
    Q_OBJECT
public:
    DiameterTransmition(QWidget *parent);

    QPushButton *gettingDiameterButton;
    QCheckBox *diemetersCheckBox, *centersCheckBox,*medianFilterCheckbox;
    QSpinBox *windowSizeSpinbox,*averageSpinbox,*reqFreqSpinbox;
    QSlider *xWindow;
private:
    QVBoxLayout *layout;
    QHBoxLayout *windowSizeLayout, *averageLayout, *reqFreqLayout,*sliderLayout;
    QLabel *windowSizeLabel, *averageLabel, *reqFreqLabel,*xWindowsLabel,*sliderValue;

signals:
    void getDiameterChanged(int state);
    void reqFreqValueChanged(int value);
    void xWindowChanged(int value);
};

#endif // DIAMETERTRANSMITION_H
