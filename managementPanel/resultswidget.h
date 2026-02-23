#ifndef RESULTSWIDGET_H
#define RESULTSWIDGET_H

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include "centerviewer.h"
#include <QFrame>
#include "collapsiblegroupbox.h"
#include "errormarker.h"

class ResultsWidget : public CollapsibleGroupBox
{
    Q_OBJECT
public:
    explicit ResultsWidget(QWidget *parent);

public slots:
    void setData(const QVector<double>& data);
    void setModel(int model);
    void setError(const QByteArray &bytes);
private:
    QVBoxLayout *layout, *errLayout;
    QHBoxLayout *hLayout;
    QLabel *diametrLabel,*diametrPlisLabel,*diametrFinalLabel,*leftShadow1Label,*rightShadow1Label,*leftShadow2Label,*rightShadow2Label,*centerPositionLabel;
    QLabel *extr1Ch1,*extr2Ch1,*extr3Ch1,*extr4Ch1,*extr1Ch2,*extr2Ch2,*extr3Ch2,*extr4Ch2;
    QLabel *shad1Ch1,*shad2Ch1,*shad1Ch2,*shad2Ch2;
    QLabel *radiusX,*radiusY,*radiusPLISX,*radiusPLISY,*radiusFinalX,*radiusFinalY;
    QLabel *ch1ShadowsLabel, *ch2ShadowsLabel;

    centerViewer *m_centerViewer;
    ErrorMarker *errorMarker1, *errorMarker2;
};

#endif // RESULTSWIDGET_H
