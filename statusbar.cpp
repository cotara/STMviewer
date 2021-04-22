#include "statusbar.h"
#include <QStatusBar>
#include <QProgressBar>
#include <QLabel>
#include <QDateTime>

StatusBar::StatusBar(QStatusBar *statusBar)
    : m_statusBar(statusBar) {

    dataReadyLabel = new QLabel("Error Status", statusBar);
    mcuLabel = new QLabel("Off-line", statusBar);
    mcuLabel->setStyleSheet(red);
    modeLabel = new QLabel("", statusBar);

    statusBar->addPermanentWidget(dataReadyLabel);
    statusBar->addPermanentWidget(mcuLabel);
    statusBar->addPermanentWidget(modeLabel);
}

void StatusBar::setStatus(bool online) {
    if (online) {
        mcuLabel->setStyleSheet(lightgreen);
        mcuLabel->setText("On-line");
    } else {
        mcuLabel->setStyleSheet(red);
        mcuLabel->setText("Off-line");
        dataReadyLabel->setText("");
        modeLabel->setText("");
    }
}

void StatusBar::setDataReady(int ready) {
    if (ready == 1) {
        dataReadyLabel->setStyleSheet(lightgreen);
        dataReadyLabel->setText("Data Ready");
    } else if (ready == 0) {
        dataReadyLabel->setStyleSheet(yellow);
        dataReadyLabel->setText("No Data");
    }
    else{
        dataReadyLabel->setStyleSheet(red);
        dataReadyLabel->setText("Error Status");
    }
}
void StatusBar::setManual(bool manual) {
    if (manual) {
        modeLabel->setText("Manual");
        modeLabel->setStyleSheet(yellow);
    } else {
        modeLabel->setText("Run");
        modeLabel->setStyleSheet(lightgreen);
    }
}

