#include "dialog.h"
#include "ui_dialog.h"

#include "filamp.h"
#include "ipedit.h"

#include <QtCore/QThread>
#include <QtWidgets/QMessageBox>

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
    , m_state(Disconnected)
{
      ui->setupUi(this);
      ui->ipEdit->setIp();
      ui->ipEdit->setPort();

      connect(ui->connectBtn, SIGNAL(clicked()), this, SLOT(updateState()));
      ui->connectBtn->installEventFilter(this);
      setWindowTitle(tr("MagicLamp Client"));

      connect(&m_tlvStream, SIGNAL(newTLV(const FiTLV &)), ui->widget, SLOT(onNewTLV(const FiTLV &)));
      connect(&m_tlvStream, SIGNAL(connectedToHost()), this, SLOT(onConnectedToHost()));
      connect(&m_tlvStream, SIGNAL(disconnectedFromHost()), this, SLOT(onDisconnectedFromHost()));
      connect(&m_tlvStream, SIGNAL(socketError(int, const QString &)), this, SLOT(onError(int, const QString &)));

      this->setFocus();
      ui->ipEdit->toolTipAnimationStart();
}

void Dialog::onConnectedToHost()
{
    setState(Connected);
    QMessageBox::information(0, tr("MagicLamp Client"),
                             tr("The host has been successfully connected."));
}

void Dialog::onDisconnectedFromHost()
{
    setState(Disconnected);
    QMessageBox::information(0, tr("MagicLamp Client"),
                             tr("The remote host has been disconnected."));
}

void Dialog::onError(int socketError, const QString &message)
{
    setState(Disconnected);
    switch (socketError) {
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::critical(0, tr("MagicLamp Client"),
                                 tr("The host was not found."));
        break;
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::critical(0, tr("MagicLamp Client"),
                                 tr("The connection was refused by the peer."));
        break;
    case QAbstractSocket::RemoteHostClosedError:
        // It is normal
        break;
    default:
        QMessageBox::critical(0, tr("MagicLamp Client"),
                                 tr("The following error occurred: %1.")
                                 .arg(message));
    }
}

Dialog::~Dialog()
{   
    m_tlvStream.abort();
    m_tlvStream.quit();
    m_tlvStream.wait();
    delete ui;
}

bool Dialog::event(QEvent *event)
{
    if(event->type() == QEvent::MouseButtonPress) {
        this->setFocus();
        ui->ipEdit->toolTipAnimationStart();
    }

    return QDialog::event(event);
}

bool Dialog::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == ui->connectBtn
        && (event->type() == QEvent::Enter || event->type() == QEvent::FocusIn)) {
        ui->ipEdit->toolTipAnimationStop();
    }

    return QDialog::eventFilter(obj, event);
}

void Dialog::setState(Dialog::State state)
{
    if(m_state == state) {
        return;
    }

    m_state = state;
    switch(m_state) {
    case Disconnected:
        ui->ipEdit->setEnabled(true);
        ui->connectBtn->setEnabled(true);
        ui->connectBtn->setText(tr("Connect"));
        break;
    case Connecting:
        ui->ipEdit->setEnabled(false);
        ui->connectBtn->setEnabled(false);
        ui->connectBtn->setText(tr("Connecting..."));
        break;
    case Connected:
        ui->ipEdit->setEnabled(false);
        ui->connectBtn->setEnabled(true);
        ui->connectBtn->setText(tr("Disconnect"));
        break;
    }
}

void Dialog::updateState()
{
    if(m_state == Disconnected) {
        m_tlvStream.connectToHost(ui->ipEdit->ip(), ui->ipEdit->port());
        setState(Connecting);
    } else if(m_state == Connected) {
        m_tlvStream.disconnectFromHost();
        setState(Disconnected);
    }
}
