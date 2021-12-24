#include "waitingdialog.h"
#include "ui_waitingdialog.h"

WaitingDialog::WaitingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WaitingDialog)
{
    ui->setupUi(this);
    setModal(true);

    hide();
    connect(&m_timer,&QTimer::timeout,this,&WaitingDialog::waitingStop);
    setWindowTitle("Ожидание...");
    mv = new QMovie(":/1/Resources/waiting.gif");
    ui->label->setMovie(mv);
}

WaitingDialog::~WaitingDialog()
{
    delete ui;
}

void WaitingDialog::waitingStop()
{
    m_timer.stop();
    hide();
    mv->stop();
}

void WaitingDialog::waitingStart(int mseconds)
{
    m_timer.start(mseconds);
    show();
    mv->start();

}
