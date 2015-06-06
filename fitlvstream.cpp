#include "fitlvstream.h"
#include "fitlv.h"
#include "fimagiccommand.h"

#include <QtCore/QtEndian>
#include <QtCore/QCoreApplication>

template <typename T>
class FiEvent: public QEvent {
public:
    explicit FiEvent(Type type)
        : QEvent(type)
    {}

    static QEvent::Type registeredType()
    {
        static int eventType = QEvent::registerEventType(); //thread-safe by Qt
        return static_cast<QEvent::Type>(eventType);
    }

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
FiTLVStream::FiTLVStream(QObject *parent)
  : QThread(parent)
  , m_tlvSocket(NULL)
{
    qRegisterMetaType<FiTLV>("FiTLV");
    qRegisterMetaType<QAbstractSocket::SocketError>("SocketError");

    moveToThread(this); // bad approach
    start();
}

void FiTLVStream::run()
{
    QTcpSocket socket;
    m_tlvSocket = &socket;

    connect(m_tlvSocket, SIGNAL(readyRead())
            , this, SLOT(onReadyRead()));
    connect(m_tlvSocket, SIGNAL(connected())
            , this, SIGNAL(connectedToHost()));
    connect(m_tlvSocket, SIGNAL(disconnected())
            , this, SIGNAL(disconnectedFromHost()));
    connect(m_tlvSocket, SIGNAL(error(QAbstractSocket::SocketError))
            , this, SLOT(onSocketError(QAbstractSocket::SocketError)));

    QThread::exec();
    m_tlvSocket = NULL;
}

bool FiTLVStream::event(QEvent *e)
{
    // so if we here, we ensure that m_tlvSocket is initialized
    // because QThread::exec() was called
    if(e->type() == FiConnectToHostEvent::registeredType()) {
        FiConnectToHostEvent *fe = static_cast<FiConnectToHostEvent*>(e);
        m_tlvSocket->connectToHost(fe->hostName(), fe->port());
        return true;
    } else if(e->type() == FiDisconnectFromHostEvent::registeredType()) {
        m_tlvSocket->disconnectFromHost();
        return true;
    } else if(e->type() == FiAbortHostEvent::registeredType()) {
        m_tlvSocket->abort();
        return true;
    }

    return QThread::event(e);
}

void FiTLVStream::connectToHost(const QString &hostName, quint16 port)
{
    QCoreApplication::postEvent(this, new FiConnectToHostEvent(hostName, port));
}

void FiTLVStream::disconnectFromHost()
{
    QCoreApplication::postEvent(this, new FiDisconnectFromHostEvent);
}

void FiTLVStream::abort()
{
    QCoreApplication::postEvent(this, new FiAbortHostEvent);
}

void FiTLVStream::onReadyRead()
{
    while (m_tlvSocket->bytesAvailable() >= 3) {

#pragma pack(push, 1)
        union {
           char b[3];
           struct {
               quint8 type;
               quint16 length;
           };
        } tlvHead;
#pragma pack(pop)

        if (m_tlvSocket->read(&tlvHead.b[0], 3) != 3) {
            onSocketError(m_tlvSocket->error());
        }
        tlvHead.length = qFromBigEndian(tlvHead.length);

        FiTLV tlvMsg;
        tlvMsg.setTypeAndLength(tlvHead.type, tlvHead.length);

        while (m_tlvSocket->bytesAvailable() < tlvMsg.length()) {
            if (!m_tlvSocket->waitForReadyRead(1000)) {
                onSocketError(m_tlvSocket->error());
            }
        }

        QByteArray tlvValue = m_tlvSocket->read(tlvMsg.length());
        if ( tlvValue.size() != tlvMsg.length()) {
            onSocketError(m_tlvSocket->error());
        }
        tlvMsg.setValue(tlvValue);
        emit newTLV(tlvMsg);
    }
}

void FiTLVStream::onSocketError(QAbstractSocket::SocketError)
{
    int error = m_tlvSocket->error();
    QString message = m_tlvSocket->errorString();
    m_tlvSocket->abort();
    emit socketError(error, message);
}
