#include "dialog.h"
#include "ui_dialog.h"

#include "filamp.h"
#include "ipedit.h"

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
      ui->setupUi(this);
      ui->ipEdit->setIp();
      ui->ipEdit->setPort();

      connect(ui->connectBtn, SIGNAL(clicked()), this, SLOT(connectToHost()));
      connect(&stream, SIGNAL(newTLV(const FiTLV &)), ui->widget, SLOT(onNewTLV(const FiTLV &)));

      setWindowTitle(tr("MagicLamp Client"));
}

void Dialog::connectToHost()
{
    stream.connectToHost(ui->ipEdit->ip(), ui->ipEdit->port());
}

Dialog::~Dialog()
{
    stream.disconnectFromHost();
    delete ui;
}
