#ifndef RESULTSWIDGET_H
#define RESULTSWIDGET_H

#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include "centerviewer.h"
#include <QFrame>

class ResultsWidget : public QGroupBox
{
    Q_OBJECT
public:
    explicit ResultsWidget(QWidget *parent);
    QVBoxLayout *layout;
    QLabel *diametrLabel,*diametrPlisLabel,*leftShadow1Label,*rightShadow1Label,*leftShadow2Label,*rightShadow2Label,*centerPositionLabel;
    QLabel *extr1Ch1,*extr2Ch1,*extr3Ch1,*extr4Ch1,*extr1Ch2,*extr2Ch2,*extr3Ch2,*extr4Ch2;
    QLabel *shad1Ch1,*shad2Ch1,*shad1Ch2,*shad2Ch2;
    QLabel *radius1,*radius2;
    QLabel *ch1ShadowsLabel, *ch2ShadowsLabel;

    centerViewer *m_centerViewer;

private:


};

#endif // RESULTSWIDGET_H
