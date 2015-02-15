#ifndef FITCPSOCKETFORTHREAD_H
#define FITCPSOCKETFORTHREAD_H

#include <QtCore/QObject>
#include <QtNetwork/QTcpSocket>

class FiTcpSocketEventFilter;
//
class FiTcpSocketForThread : public QObject
{
    Q_OBJECT

public:
    explicit FiTcpSocketForThread(QTcpSocket *tcpSocket, QObject *parent = 0);
    bool event(QEvent * e);
    QTcpSocket *operator->() const {return m_tcpSocket;}
    operator QObject*() const {return m_tcpSocket;}

public slots:
    void connectToHost(const QString &hostName, quint16 port);
    void disconnectFromHost();
    void abort();

public:
    QTcpSocket *m_tcpSocket;
    FiTcpSocketEventFilter *m_eventFilter;

};
//
class FiTcpSocketEventFilter: public QObject
{
    Q_OBJECT

public:
    explicit FiTcpSocketEventFilter(QTcpSocket *tcpSocket, QObject *parent = 0);
    bool eventFilter(QObject *obj, QEvent * e);

private:
    QTcpSocket *m_tcpSocket;

};

#endif // FITCPSOCKETFORTHREAD_H
