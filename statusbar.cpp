#include "statusbar.h"
#include <QStatusBar>
#include <QProgressBar>
#include <QLabel>
#include <QDateTime>

StatusBar::StatusBar(QStatusBar *statusBar)
    : m_statusBar(statusBar) {

    timeLabel = new QLabel("", statusBar);
    mcuLabel = new QLabel("Off-line", statusBar);
    mcuLabel->setStyleSheet(red);
    modeLabel = new QLabel("", statusBar);

    statusBar->addPermanentWidget(timeLabel);
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
        timeLabel->setText("");
        modeLabel->setText("");
    }
}

void StatusBar::setTime(const QDateTime &time) {
    timeLabel->setText(time.toString());
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

