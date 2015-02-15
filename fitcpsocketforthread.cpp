#include "fitcpsocketforthread.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QThread>

template <typename T>
class FiEvent: public QEvent {
public:
    explicit FiEvent(Type type)
        : QEvent(type)
    {}

    static QEvent::Type registeredType()
    {
        static int eventType = QEvent::registerEventType();
        return static_cast<QEvent::Type>(eventType);
    }

};

class FiThreadChangedEvent: public FiEvent<FiThreadChangedEvent> {
public:
    FiThreadChangedEvent(QThread *prevThread)
        : FiEvent(registeredType())
        , m_prevThread(prevThread)
    {}

    QThread *prevThread() const {return m_prevThread;}

    static QEvent::Type registeredType()
    {
        static int eventType = QEvent::registerEventType();
        return static_cast<QEvent::Type>(eventType);
    }

private:
    QThread *m_prevThread;

};

class FiMoveToThreadEvent: public FiEvent<FiThreadChangedEvent> {
public:
    FiMoveToThreadEvent(QThread *targetThread)
        : FiEvent(registeredType())
        , m_targetThread(targetThread)
    {}

    QThread *targetThread() const {return m_targetThread;}

    static QEvent::Type registeredType()
    {
        static int eventType = QEvent::registerEventType();
        return static_cast<QEvent::Type>(eventType);
    }

private:
    QThread *m_targetThread;

};

class FiConnectToHostEvent: public FiEvent<FiConnectToHostEvent>  {
public:
    FiConnectToHostEvent(const QString &hostName, quint16 port)
        : FiEvent(registeredType())
        , m_hostName(hostName)
        , m_port(port)
    {}

    const QString &hostName() const {return m_hostName;}
    quint16 port() const {return m_port;}

private:
    QString m_hostName;
    quint16 m_port;

};

class FiDisconnectFromHostEvent: public FiEvent<FiDisconnectFromHostEvent>  {
public:
    FiDisconnectFromHostEvent()
        : FiEvent(registeredType())
    {}

};

class FiAbortHostEvent: public FiEvent<FiAbortHostEvent>  {
public:
    FiAbortHostEvent()
        : FiEvent(registeredType())
    {}

};
//
FiTcpSocketForThread::FiTcpSocketForThread(QTcpSocket *tcpSocket, QObject *parent)
    : QObject(parent)
    , m_tcpSocket(tcpSocket)
    , m_eventFilter(new FiTcpSocketEventFilter(m_tcpSocket, this))
{
    connect(this, SIGNAL(destroyed()), m_tcpSocket, SLOT(deleteLater()));
}

bool FiTcpSocketForThread::event(QEvent *e)
{
    if(e->type() == FiThreadChangedEvent::registeredType()) {
        FiMoveToThreadEvent fe(QThread::currentThread());
        QCoreApplication::sendEvent(m_tcpSocket, &fe);
        return true;
    } else if(e->type() == QEvent::ThreadChange) {
        // Qt garanteese that any new posted events will be handled in the targetThread
        FiThreadChangedEvent fe(QThread::currentThread());
        QCoreApplication::sendEvent(this, &fe);
    }
    return QObject::event(e);
}

void FiTcpSocketForThread::connectToHost(const QString &hostName, quint16 port)
{
    QCoreApplication::postEvent(m_tcpSocket, new FiConnectToHostEvent(hostName, port));
}

void FiTcpSocketForThread::disconnectFromHost()
{
    QCoreApplication::postEvent(m_tcpSocket, new FiDisconnectFromHostEvent);
}

void FiTcpSocketForThread::abort()
{
    QCoreApplication::postEvent(m_tcpSocket, new FiAbortHostEvent);
}
//
FiTcpSocketEventFilter::FiTcpSocketEventFilter(QTcpSocket *tcpSocket, QObject *parent)
    : QObject(parent)
    , m_tcpSocket(tcpSocket)
{
    m_tcpSocket->installEventFilter(this);
}

bool FiTcpSocketEventFilter::eventFilter(QObject *obj, QEvent *e)
{
    if (obj != m_tcpSocket) {
        return QObject::eventFilter(obj, e);
    }

    if(e->type() == FiMoveToThreadEvent::registeredType()) {
        FiMoveToThreadEvent *fe = static_cast<FiMoveToThreadEvent*>(e);
        disconnect(m_tcpSocket->thread(), SIGNAL(finished()), m_tcpSocket, SLOT(deleteLater()));
        connect(fe->targetThread(), SIGNAL(finished()), m_tcpSocket, SLOT(deleteLater()));
        m_tcpSocket->moveToThread(fe->targetThread());
        this->moveToThread(fe->targetThread());
        return true;
   } else if(e->type() == FiConnectToHostEvent::registeredType()) {
        FiConnectToHostEvent *fe = static_cast<FiConnectToHostEvent*>(e);
        m_tcpSocket->connectToHost(fe->hostName(), fe->port());
        return true;
    } else if(e->type() == FiDisconnectFromHostEvent::registeredType()) {
        m_tcpSocket->disconnectFromHost();
        return true;
    } else if(e->type() == FiAbortHostEvent::registeredType()) {
        m_tcpSocket->abort();
        return true;
    }

    return false;
}
