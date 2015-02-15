#include "filamp.h"
#include "fitlv.h"
#include "fimagiccommand.h"

#include <QtGui>

FiLamp::FiLamp(QWidget *parent)
    : QWidget(parent)
    , m_isOn(false)
    , m_mainEllipse(QColor(), 0)
{
    setFixedSize(300, 300);

    m_timer.setInterval(100);
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(onTimer()));

    m_timer.start();
}

void FiLamp::onNewTLV(const FiTLV &tlv)
{
    switch(tlv.type()) {
    case FiMagicCommand::ON: {
        m_isOn = true;
    } break;

    case FiMagicCommand::OFF: {
        m_isOn = false;

        m_ellipses.push_front(Ellipse(m_mainEllipse.color, m_mainEllipse.radius));
        m_mainEllipse.color = QColor();
    } break;

    case FiMagicCommand::COLOR: {
        if (m_isOn) {
            FiCommandColor tlvColor;
            tlvColor.setDataFromTLV(tlv);
            m_ellipses.push_back(Ellipse(tlvColor.color(), 10));
        }
    } break;
    }
    update();
}

void FiLamp::onTimer()
{
    if (m_isOn) {
        int count = 0;
        foreach (const Ellipse &e, m_ellipses) {
            e.radius += 5;
            if (e.radius >= qMin(width()/2, height()/2)) {
                ++count;
            }
        }
        for (int i = 0; i < count; ++i) {
            m_mainEllipse = m_ellipses.first();
            m_ellipses.pop_front();
        }
    } else {
        int count = 0;
        foreach (const Ellipse &e, m_ellipses) {
            e.radius -= 10;
            if (e.radius <= 0) {
                ++count;
            }
        }
        for (int i = 0; i < count; ++i) {
            m_ellipses.pop_back();
        }
    }
    update();
}

void FiLamp::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    if (m_isOn) {
        painter.setBrush(m_mainEllipse.color);
        painter.drawEllipse(0, 0, width(), height());
        foreach (const Ellipse &e, m_ellipses) {
            painter.setBrush(e.color);
            painter.drawEllipse(QPoint(width()/2, height()/2), e.radius, e.radius);
        }
    } else {
        painter.setBrush(QColor(Qt::black));
        painter.drawEllipse(QPoint(width()/2, height()/2), width()/2, height()/2);
        foreach (const Ellipse &e, m_ellipses) {
            painter.setBrush(e.color);
            painter.drawEllipse(QPoint(width()/2, height()/2), e.radius, e.radius);
        }
        if (m_ellipses.isEmpty()) {
            painter.setBrush(QColor(Qt::white));
            painter.drawEllipse(QPoint(width()/2, height()/2), 3, 3);
        }
    }
}
