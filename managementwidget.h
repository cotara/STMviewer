#ifndef MANAGEMENTWIDGET_H
#define MANAGEMENTWIDGET_H

#include <QWidget>
#include "plissettings.h"
#include "transmitionsettings.h"
#include "historysettings.h"
#include "diametertransmition.h"

class ManagementWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ManagementWidget(QWidget *parent = nullptr);
    PlisSettings *m_plisSettings;
    TransmitionSettings *m_TransmitionSettings;
    HistorySettings *m_HistorySettings;
    DiameterTransmition *m_DiameterTransmition;
signals:

private:
    QVBoxLayout *layout;

};

#endif // MANAGEMENTWIDGET_H
