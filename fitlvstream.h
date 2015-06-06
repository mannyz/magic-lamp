#ifndef FITLVSTREAM_H
#define FITLVSTREAM_H

#include "fitlv.h"

#include <QtCore/QThread>
#include <QtNetwork/QTcpSocket>

class FiTLVStream : public QThread
{
    Q_OBJECT

public:
    explicit FiTLVStream(QObject *parent = 0);
    bool event(QEvent * e);

protected:
    void run();

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
    void onReadyRead();
    void onSocketError(QAbstractSocket::SocketError);

private:
    QTcpSocket *m_tlvSocket;

};

#endif // FITLVSTREAM_H
