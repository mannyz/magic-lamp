#ifndef IPEDIT_H
#define IPEDIT_H

#include <QFrame>
#include <QTimer>
#include <QVector>

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

public slots:
    void toolTipAnimationStart();
    void toolTipAnimationStop();

private slots:
    void onCheckOctet(int i);
    void onCheckAcceptIp();
    //
    void toolTipAnimationUpdate();
    void toolTipAnimationRestart();

private:
    QLineEdit *m_octetEdit[4];
    QLineEdit *m_portEdit;

private:
    bool m_isToolTipAnimationAllowed;
    bool m_isToolTipAnimationRun;
    QTimer m_toolTipAnimationTimer;
    QVector<QString> m_toolTipAnimationEvents;
    int m_ToolTipAnimationStep;
    QString m_toolTipAnimationPrevIp;


};

#endif // IPEDIT_H
