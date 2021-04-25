#include "statusbar.h"
#include <QStatusBar>
#include <QProgressBar>
#include <QLabel>
#include <QDateTime>
#include <QProgressBar>


StatusBar::StatusBar(QStatusBar *statusBar)
    : m_statusBar(statusBar) {

    dataReadyLabel = new QLabel("", statusBar);
    mcuLabel = new QLabel("Off-line", statusBar);
    mcuLabel->setStyleSheet(red);
    infoLabel = new QLabel("", statusBar);
    downloadGif = new QMovie(":/1/Resources/unnamed2.gif");
    downloadLabel = new QLabel("",statusBar);


    downloadLabel->setMovie(downloadGif);
    downloadLabel->setMinimumWidth(20);
    statusBar->addPermanentWidget(downloadLabel);

    statusBar->addPermanentWidget(dataReadyLabel);
    statusBar->addPermanentWidget(mcuLabel);
    statusBar->addPermanentWidget(infoLabel);


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
void StatusBar::setInfo(int info) {
    infoLabel->setText(QString::number(info));
}

void StatusBar::setDownloadGif(bool downloading)
{
    if(downloading){
        downloadLabel->setMovie(downloadGif);
        downloadGif->setSpeed(200);
        downloadGif->start();
    }
    else
        downloadLabel->clear();
        //downloadGif->stop();

}

