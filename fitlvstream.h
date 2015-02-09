#ifndef FITLVSTREAM_H
#define FITLVSTREAM_H

#include <QObject>
#include <QtNetwork>

#include "fitlv.h"

class FiTLVStream : public QObject
{
    Q_OBJECT
public:
    explicit FiTLVStream(QObject *parent = 0);

signals:
    void newTLV(const FiTLV &tlv);

public slots:
    void connectToHost(const QString &hostName, quint16 port);
    void disconnectFromHost();

private slots:
    void readTLV();
    void onConnected();
    void onDisconnected();
    void displayError(QAbstractSocket::SocketError socketError);

private:
    QTcpSocket *m_tcpSocket;
    FiTLV m_tlvBuf;
    bool m_wantDisconnect;

};

#endif // FITLVSTREAM_H
