#include "statusbar.h"
#include <QStatusBar>
#include <QProgressBar>
#include <QLabel>
#include <QDateTime>
#include <QProgressBar>


StatusBar::StatusBar(QStatusBar *statusBar)
    : m_statusBar(statusBar) {


    dataReadyLabel = new QLabel("Error Status", statusBar);
    dataReadyLabel->setStyleSheet(red);
    mcuLabel = new QLabel("Off-line", statusBar);
    mcuLabel->setStyleSheet(red);
    infoLabel = new QLabel("", statusBar);
    downloadBar = new QProgressBar();
    reSent = new QLabel("Переотправлено: ", statusBar);

    statusBar->addPermanentWidget(reSent);
    downloadBar->setMinimumWidth(100);
    downloadBar->setMinimumHeight(2);
    statusBar->addPermanentWidget(downloadBar);

    statusBar->addPermanentWidget(dataReadyLabel);
    dataReadyLabel->setMinimumWidth(50);
    dataReadyLabel->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    dataReadyLabel->setAlignment(Qt::AlignHCenter);

    statusBar->addPermanentWidget(mcuLabel);
    mcuLabel->setMinimumWidth(50);
    mcuLabel->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    mcuLabel->setAlignment(Qt::AlignHCenter);

    statusBar->addPermanentWidget(infoLabel);
    infoLabel->setMinimumWidth(20);
    infoLabel->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    infoLabel->setAlignment(Qt::AlignHCenter);
    infoLabel->setVisible(false);
}


void StatusBar::setStatus(bool online) {
    if (online) {
        mcuLabel->setStyleSheet(lightgreen);
        mcuLabel->setText("On-line");
    } else {
        mcuLabel->setStyleSheet(red);
        mcuLabel->setText("Off-line");
    }
}

void StatusBar::setDataReady(int ready) {
    if (ready < 0) {
        dataReadyLabel->setStyleSheet(red);
        dataReadyLabel->setText("Error Status");

    } else if (ready == 0) {
        dataReadyLabel->setStyleSheet(yellow);
        dataReadyLabel->setText("No Data");
    }
    else{
        dataReadyLabel->setStyleSheet(lightgreen);
        dataReadyLabel->setText(QString::number(ready));
    }
}
void StatusBar::setInfo(int info) {
    infoLabel->setText(QString::number(info));
}

void StatusBar::setDownloadBarRange(int value){
    downloadBar->setRange(0,value);
}

int StatusBar::getDownloadBarRange(){
    return downloadBar->maximum();

}

int StatusBar::getDownloadBarValue()
{
    return downloadBar->value();
}

void StatusBar::incReSent(){
    reSent->setText("Переотправлено: " + QString::number(++reSentCount));
}

void StatusBar::clearReSent(){
    reSentCount=0;
    reSent->setText("Переотправлено: ");
}

void StatusBar::setMessageBar(QString status){
    m_statusBar->showMessage(status);
}


void StatusBar::setDownloadBarValue(int value){
    downloadBar->setValue(value);
}

void StatusBar::incDownloadBarValue(int value)
{
    downloadBar->setValue(getDownloadBarValue()+value);
}

