#include "fitlvstream.h"
#include "fitlv.h"
#include "fimagiccommand.h"

#include <QtCore/QtEndian>

FiTLVStream::FiTLVStream(QObject *parent)
  : QObject(parent)
  , m_tcpSocket(new QTcpSocket)
{
    qRegisterMetaType<FiTLV>("FiTLV");
    qRegisterMetaType<QAbstractSocket::SocketError>("SocketError");

    connect(m_tcpSocket, SIGNAL(readyRead())
            , this, SLOT(readTLV()));
    connect(m_tcpSocket, SIGNAL(connected())
            , this, SIGNAL(connectedToHost()));
    connect(m_tcpSocket, SIGNAL(disconnected())
            , this, SIGNAL(disconnectedFromHost()));
    connect(m_tcpSocket, SIGNAL(error(QAbstractSocket::SocketError))
            , this, SLOT(onSocketError(QAbstractSocket::SocketError)));

}

void FiTLVStream::connectToHost(const QString &hostName, quint16 port)
{
    m_tcpSocket.connectToHost(hostName, port);
}

void FiTLVStream::disconnectFromHost()
{
    m_tcpSocket.disconnectFromHost();
}

void FiTLVStream::abort()
{
    m_tcpSocket.abort();
}

void FiTLVStream::readTLV()
{
    if (m_tlvBuf) {
        if (m_tcpSocket->bytesAvailable() < 3) {
            return;
        }

        quint8 tlvType;
        if (!m_tcpSocket->getChar((char*)&tlvType)) {
            onSocketError(m_tcpSocket->error());
        }

        quint16 tlvLength;
        if (m_tcpSocket->read((char*)&tlvLength, 2) != 2) {
            onSocketError(m_tcpSocket->error());
        }
        tlvLength = qFromBigEndian(tlvLength);

        m_tlvBuf.setTypeAndLength(tlvType, tlvLength);
    }

    if (m_tcpSocket->bytesAvailable() < m_tlvBuf.length()) {
        return;
    }

    QByteArray tlvValue = m_tcpSocket->read(m_tlvBuf.length());
    if ( tlvValue.size() != m_tlvBuf.length()) {
        onSocketError(m_tcpSocket->error());
    }
    m_tlvBuf.setValue(tlvValue);
    emit newTLV(m_tlvBuf);

    m_tlvBuf.clear();
}

void FiTLVStream::onSocketError(QAbstractSocket::SocketError)
{
    int error = m_tcpSocket->error();
    QString message = m_tcpSocket->errorString();
    m_tcpSocket.abort();
    emit socketError(error, message);
}
