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
#include <QProgressBar>
#include <QRadioButton>

class DiameterTransmition : public QGroupBox
{
    Q_OBJECT
public:
    DiameterTransmition(QWidget *parent);

    QPushButton *gettingDiameterButton;
    QCheckBox *diemetersCheckBox, *centersCheckBox,*medianFilterCheckbox;
    QSpinBox *windowSizeSpinbox,*averageSpinbox,*reqFreqSpinbox;
    QSlider *xWindow;
    QProgressBar *progressBar;
    QRadioButton *continiousMode, *collectMode;
    QLabel *countPointsLabel;
    QSpinBox * countPointsBox;
private:
    QVBoxLayout *layout;
    QHBoxLayout *windowSizeLayout, *averageLayout, *reqFreqLayout,*sliderLayout,*radioLayout,*colectLayout;
    QLabel *windowSizeLabel, *averageLabel, *reqFreqLabel,*xWindowsLabel,*sliderValue;

signals:
    void getDiameterChanged(int state);
    void reqFreqValueChanged(int value);
    void xWindowChanged(int value);
    void countPointsChanged(int value);
    void diameterModeChanged(bool mode);
};

#endif // DIAMETERTRANSMITION_H
