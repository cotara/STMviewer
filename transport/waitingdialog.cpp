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
}

WaitingDialog::~WaitingDialog()
{
    delete ui;
}

void WaitingDialog::waitingStop()
{
    m_timer.stop();
    hide();
}

void WaitingDialog::waitingStart(int mseconds)
{
    m_timer.start(mseconds);
    show();
}
