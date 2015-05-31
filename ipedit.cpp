#include "ipedit.h"

#include <QLineEdit>
#include <QHBoxLayout>
#include <QLabel>
#include <QKeyEvent>
#include <QRegExpValidator>

#include <QtCore/QCoreApplication>
#include <QDebug>

IPEdit::IPEdit(QWidget *parent)
    : QFrame(parent)
{
    setContentsMargins(0, 0, 0, 0);
    setStyleSheet("QLabel {background: white}");

    QHBoxLayout *ipLayout = new QHBoxLayout(this);
    ipLayout->setContentsMargins(0, 0, 0, 0);
    ipLayout->setSpacing(0);

    QRegExp ipRx("^(?:0|[1-9]|[1-9][0-9]|1[0-9][0-9]|2([0-4][0-9]|5[0-5]))$");
    QValidator *ipV = new QRegExpValidator(ipRx, this);

    for (int i = 0; i < 4; ++i) {
        if (i != 0) {
            QLabel *l = new QLabel(".", this);
            l->setFixedHeight(22);
            l->setFixedWidth(3);
            ipLayout->addWidget(l);

        }

        m_octetEdit[i] = new QLineEdit(this);
        m_octetEdit[i]->setFrame(false);
        m_octetEdit[i]->setAlignment(Qt::AlignCenter);
        m_octetEdit[i]->setFixedHeight(22);
        m_octetEdit[i]->setFixedWidth(30);
        m_octetEdit[i]->setValidator(ipV);
        m_octetEdit[i]->installEventFilter(this);

        ipLayout->addWidget(m_octetEdit[i]);
    }

    QLabel *l = new QLabel(":", this);
    l->setFixedHeight(22);
    l->setFixedWidth(3);
    ipLayout->addWidget(l);
    m_portEdit = new QLineEdit(this);
    m_portEdit->setFrame(false);
    m_portEdit->setAlignment(Qt::AlignCenter);
    m_portEdit->setFixedHeight(22);
    m_portEdit->setFixedWidth(40);
    QRegExp portRx("^(?:102[4-9]|1[1-9]{3}|[2-9][1-9]{3}|[1-6][1-5][1-5][1-3][1-5])$");
    QValidator *portV = new QRegExpValidator(portRx, this);
    m_portEdit->setValidator(portV);
    m_portEdit->installEventFilter(this);
    ipLayout->addWidget(m_portEdit);

    setLayout(ipLayout);
    setFixedHeight(22);
    setFixedWidth(200);

    connect(this, SIGNAL(checkOctet(int)), this, SLOT(onCheckOctet(int)), Qt::QueuedConnection);
    connect(this, SIGNAL(checkAcceptIp()), this, SLOT(onCheckAcceptIp()), Qt::QueuedConnection);

    //tooltip
    connect(&m_toolTipAnimationTimer, SIGNAL(timeout())
            , this, SLOT(toolTipAnimationUpdate()));
    m_toolTipAnimationTimer.setInterval(500);
    m_toolTipAnimationEvents.push_back("12");
    m_toolTipAnimationEvents.push_back("1");
    m_toolTipAnimationEvents.push_back("");
    m_toolTipAnimationEvents.push_back("1");
    m_toolTipAnimationEvents.push_back("12");
    m_toolTipAnimationEvents.push_back("127");
    m_ToolTipAnimationStep = 0;
    m_isToolTipAnimationRun = false;
}

void IPEdit::setIp(const QString &ip)
{
    QStringList octets = ip.split(".");
    for (int i = 0; i < 4; ++i) {
        m_octetEdit[i]->setText(octets[i]);
    }
    emit checkAcceptIp();
}

void IPEdit::setPort(const QString &port)
{
    m_portEdit->setText(port);
}

QString IPEdit::ip() const
{
    return QString("%1.%2.%3.%4").arg(m_octetEdit[0]->text()).arg(m_octetEdit[1]->text()).arg(m_octetEdit[2]->text()).arg(m_octetEdit[3]->text());
}

quint16 IPEdit::port() const
{
    return m_portEdit->text().toInt();
}

bool IPEdit::eventFilter(QObject *obj, QEvent *event)
{
    if (QFrame::eventFilter(obj, event)) {
        return true;
    }

    if (event->type() == QEvent::KeyPress) {
        if (m_portEdit == obj) {
            emit checkAcceptIp();
        } else {
            for (int i = 0; i < 4; ++i) {
                if (m_octetEdit[i] == obj) {
                       QKeyEvent *ke = static_cast<QKeyEvent*>(event);
                       switch (ke->key()) {
                       case Qt::Key_Left:
                           if (m_octetEdit[i]->cursorPosition() == 0) {
                               moveToLeftOctet(i);
                           } break;

                       case Qt::Key_Right:
                           if (m_octetEdit[i]->text().size() == m_octetEdit[i]->cursorPosition()
                               || m_octetEdit[i]->text().isEmpty()) {
                               moveToRightOctet(i);
                           } break;

                       case Qt::Key_Backspace:
                           if (m_octetEdit[i]->text().isEmpty()
                                   || m_octetEdit[i]->cursorPosition() == 0) {
                               moveToLeftOctet(i);
                           }
                           emit checkAcceptIp();
                           break;

                       case Qt::Key_Period:
                       case Qt::Key_Comma:
                           moveToRightOctet(i);
                           break;

                       case Qt::Key_0:
                           if (m_octetEdit[i]->text().isEmpty()
                                   || m_octetEdit[i]->text() == "0") {
                               moveToRightOctet(i);
                           }
                       default:
                           emit checkOctet(i);
                           emit checkAcceptIp();
                           break;

                       }
                }
            }
        }
    } else if (event->type() == QEvent::FocusIn) {
        toolTipAnimationStop();
    }

    return false;
}

void IPEdit::moveToRightOctet(int i)
{
    if (i+1 < 4) {
        m_octetEdit[i+1]->setFocus();
        m_octetEdit[i+1]->setCursorPosition(0);
        m_octetEdit[i+1]->selectAll();
    } else if (i == 3) {
        m_portEdit->setFocus();
        m_portEdit->setCursorPosition(0);
        m_portEdit->selectAll();
    }
}

void IPEdit::moveToLeftOctet(int i)
{
    if (i > 0) {
        m_octetEdit[i-1]->setFocus();
        m_octetEdit[i-1]->setCursorPosition(m_octetEdit[i-1]->text().size());
    }
}

void IPEdit::onCheckOctet(int i)
{
    if ((m_octetEdit[i]->text().size() == 3 && m_octetEdit[i]->text().size() == m_octetEdit[i]->cursorPosition())
          || (m_octetEdit[i]->text().size() == 2 && m_octetEdit[i]->text().toInt() > 25)
          || (m_octetEdit[i]->text() == "0")) {
           moveToRightOctet(i);
    }
}

void IPEdit::onCheckAcceptIp()
{
    bool ipOk = true;
    int pos = 0;
    QString text;
    for (int i = 0; i < 4; ++i) {
        pos = 0;
        text = m_octetEdit[i]->text();
        ipOk &= (m_octetEdit[i]->validator()->validate(text, pos) != QValidator::Invalid);
        ipOk &= !m_octetEdit[i]->text().isEmpty();
    }
    pos = 0;
    text = m_portEdit->text();
    ipOk &= (m_portEdit->validator()->validate(text, pos) == QValidator::Acceptable);
    ipOk &= !m_portEdit->text().isEmpty();
    if (ipOk) {
        setStyleSheet( "* {background: limegreen}" );
    } else {
        setStyleSheet( "* {background: hotpink }" );
    }
}

void IPEdit::toolTipAnimationUpdate()
{
    if (!m_isToolTipAnimationRun) {
        return;
    }

    if(m_ToolTipAnimationStep == m_toolTipAnimationEvents.size()) {
        toolTipAnimationRestart();
        return;
    }

    m_octetEdit[0]->setText(m_toolTipAnimationEvents[m_ToolTipAnimationStep]);
    ++m_ToolTipAnimationStep;
    checkAcceptIp();
}

void IPEdit::toolTipAnimationStart()
{
    if(m_isToolTipAnimationRun) {
        return;
    }
    m_toolTipAnimationPrevIp = ip();
    setIp("127.0.0.1");
    m_isToolTipAnimationRun = true;
    m_ToolTipAnimationStep = 0;
    for (int i = 0; i < 4; ++i) {
        m_octetEdit[i]->clearFocus();
    }
    m_toolTipAnimationTimer.start();
}

void IPEdit::toolTipAnimationStop()
{
    if(!m_isToolTipAnimationRun) {
        return;
    }
    m_isToolTipAnimationRun = false;
    setIp(m_toolTipAnimationPrevIp);
    m_toolTipAnimationTimer.stop();
    m_ToolTipAnimationStep = 0;
}

void IPEdit::toolTipAnimationRestart()
{
    if(!m_isToolTipAnimationRun) {
        return;
    }
    m_ToolTipAnimationStep = 0;
}
