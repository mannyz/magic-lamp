#ifndef IPEDIT_H
#define IPEDIT_H

#include <QFrame>

QT_BEGIN_NAMESPACE
class QLineEdit;
QT_END_NAMESPACE

class IPEdit: public QFrame
{
    Q_OBJECT

public:
    IPEdit(QWidget *parent = 0);

    void setIp(const QString &ip = "127.0.0.1");
    void setPort(const QString &port = "9999");

    QString ip() const;
    quint16 port() const;

public:
    virtual bool eventFilter(QObject *obj, QEvent *event);

private:
    void init();
    void moveToRightOctet(int i);
    void moveToLeftOctet(int i);

signals:
    void checkOctet(int i);
    void checkAcceptIp();

private slots:
    void onCheckOctet(int i);
    void onCheckAcceptIp();

private:
    QLineEdit *m_octetEdit[4];
    QLineEdit *m_portEdit;


};

#endif // IPEDIT_H
