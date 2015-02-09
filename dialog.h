#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

#include "fitlvstream.h"

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

private slots:
    void connectToHost();

private:
    Ui::Dialog *ui;
    FiTLVStream stream;

};
#endif // DIALOG_H
