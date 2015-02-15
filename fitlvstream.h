#ifndef FITLVSTREAM_H
#define FITLVSTREAM_H

#include "fitlv.h"
#include "fitcpsocketforthread.h"

#include <QtCore/QObject>

class FiTLVStream : public QObject
{
    Q_OBJECT

public:
    explicit FiTLVStream(QObject *parent = 0);

signals:
    void newTLV(const FiTLV &tlv);
    void connectedToHost();
    void disconnectedFromHost();
    void socketError(int error, const QString &message);

public slots:
    void connectToHost(const QString &hostName, quint16 port);
    void disconnectFromHost();
    void abort();

private slots:
    void readTLV();
    void onSocketError(QAbstractSocket::SocketError socketError);

private:
    FiTcpSocketForThread m_tcpSocket;
    FiTLV m_tlvBuf;

};

#endif // FITLVSTREAM_H
