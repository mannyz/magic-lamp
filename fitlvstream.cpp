#include "fitlvstream.h"
#include "fitlv.h"
#include "fimagiccommand.h"

#include <QtCore/QtEndian>
#include <QtWidgets/QMessageBox>

FiTLVStream::FiTLVStream(QObject *parent)
  : QObject(parent)
  , m_wantDisconnect(false)
{
    m_tlvBuf.setType(FiMagicCommand::UNKNOWN);

    m_tcpSocket = new QTcpSocket(this);

    connect(m_tcpSocket, SIGNAL(readyRead()), this, SLOT(readTLV()));
    connect(m_tcpSocket, SIGNAL(connected()), this, SLOT(onConnected()));
    connect(m_tcpSocket, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
    connect(m_tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(displayError(QAbstractSocket::SocketError)));

}

void FiTLVStream::connectToHost(const QString &hostName, quint16 port)
{
    m_wantDisconnect = false;
    m_tcpSocket->abort();
    m_tcpSocket->connectToHost(hostName, port);
}

void FiTLVStream::disconnectFromHost()
{
    m_wantDisconnect = true;
    m_tcpSocket->disconnectFromHost();
}

void FiTLVStream::readTLV()
{
    if (m_tlvBuf.type() == FiMagicCommand::UNKNOWN) {
        if (m_tcpSocket->bytesAvailable() < 3) {
            return;
        }

        quint8 tlvType;
        if (!m_tcpSocket->getChar((char*)&tlvType)) {
            displayError(m_tcpSocket->error());
        }

        quint16 tlvLength;
        if (m_tcpSocket->read((char*)&tlvLength, 2) != 2) {
            displayError(m_tcpSocket->error());
        }
        tlvLength = qFromBigEndian(tlvLength);

        m_tlvBuf.setType(tlvType);
        m_tlvBuf.setLength(tlvLength);
    }

    if (m_tcpSocket->bytesAvailable() < m_tlvBuf.length()) {
        return;
    }

    QByteArray tlvValue = m_tcpSocket->read(m_tlvBuf.length());
    if ( tlvValue.size() != m_tlvBuf.length()) {
        displayError(m_tcpSocket->error());
    }
    m_tlvBuf.setValue(tlvValue);
    emit newTLV(m_tlvBuf);

    m_tlvBuf.setType(FiMagicCommand::UNKNOWN);
}

void FiTLVStream::onConnected()
{
    QMessageBox::information(0, tr("MagicLamp Client"),
                             tr("The host has been successfully connected."));
}

void FiTLVStream::onDisconnected()
{
    FiTLV tlv;
    tlv.setType(FiMagicCommand::OFF);
    emit newTLV(tlv);
    if (!m_wantDisconnect) {
        QMessageBox::information(0, tr("MagicLamp Client"),
                                 tr("The host has been disconnected."));
    }
}

void FiTLVStream::displayError(QAbstractSocket::SocketError socketError)
{
    m_tcpSocket->abort();

    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::information(0, tr("MagicLamp Client"),
                                 tr("The host was not found."));
        break;
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::information(0, tr("MagicLamp Client"),
                                 tr("The connection was refused by the peer."));
        break;
    default:
        QMessageBox::information(0, tr("MagicLamp Client"),
                                 tr("The following error occurred: %1.")
                                 .arg(m_tcpSocket->errorString()));
    }
}
