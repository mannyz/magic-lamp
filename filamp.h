#ifndef FILAMP_H
#define FILAMP_H

#include <QWidget>
#include <QQueue>
#include <QTimer>

class FiTLV;

class FiLamp : public QWidget
{
    Q_OBJECT
public:
    explicit FiLamp(QWidget *parent = 0);

protected:
    virtual void paintEvent(QPaintEvent *);

signals:

public slots:
    void onNewTLV(const FiTLV &tlv);

private slots:
    void onTimer();

private:
    class Ellipse{
    public:
        Ellipse(const QColor &c, int r)
            : color(c)
            , radius(r)
        {}

        QColor color;
        mutable int radius;
    };

    bool m_isOn;

    QQueue<Ellipse> m_ellipses;
    Ellipse m_mainEllipse;
    QTimer m_timer;

};

#endif // FILAMP_H
