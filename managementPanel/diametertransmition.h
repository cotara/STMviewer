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
    QCheckBox *diemetersCheckBox, *centersCheckBox,*medianFilterCheckbox, *furieCheckbox;
    QSpinBox *windowSizeSpinbox,*averageSpinbox,*limitSpinbox, *reqFreqSpinbox, *furieLimitSpinbox;
    QSlider *xWindow;
    QRadioButton *continiousMode, *collectMode;
    QLabel *r1ValueLabel,*r2ValueLabel;
    QLabel *collectCountLabel;

private:
    QVBoxLayout *layout;
    QHBoxLayout *windowSizeLayout, *averageLayout,*limitLayout, *reqFreqLayout,*sliderLayout,*radioLayout,*colectLayout,*r1HLayout, *r2HLayout, *furieLimitLayout;
    QLabel *windowSizeLabel, *averageLabel, *limitLabel, *reqFreqLabel,*xWindowsLabel,*sliderValue, *furieLimitLabel;
    QLabel *r1Label,*r2Label;
    QLabel *collectLabel;

signals:
    void getDiameterChanged(int state);
    void reqFreqValueChanged(int value);
    void xWindowChanged(int value);
    void diameterModeChanged(bool mode);
    void windowSizeChanged(int value);
    void averageChanged(int value);
    void limitChanged(int value);
    void furieLimitChanged(int value);
};

#endif // DIAMETERTRANSMITION_H
