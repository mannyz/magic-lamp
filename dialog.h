#ifndef DIALOG_H
#define DIALOG_H

#include "fitlvstream.h"
#include <QtWidgets/QDialog>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

    enum State {
        Disconnected,
        Connecting,
        Connected
    };

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();
    virtual bool event(QEvent *event);
    virtual bool eventFilter(QObject *obj, QEvent *event);

private slots:
    void setState(State state);
    void updateState();
    void onConnectedToHost();
    void onDisconnectedFromHost();
    void onError(int socketError, const QString &message);

private:
    Ui::Dialog *ui;

private:
    FiTLVStream m_tlvStream;
    State m_state;

};
#endif // DIALOG_H
