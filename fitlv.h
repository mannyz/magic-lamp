#ifndef FITLV_H
#define FITLV_H

#include <QtCore/QObject>
#include <QtCore/QtGlobal>
#include <QtCore/QByteArray>

class FiTLV
{
public:
    FiTLV();

    quint8 type() const {return m_type;}
    quint16 length() const {return m_value.size();}
    void setTypeAndLength(quint8 type, quint16 length = 0) {m_type = type; m_value.resize(length); m_isClear = false;}
    const QByteArray &value() const {return m_value;}
    void setValue(const QByteArray &value) {m_value = value;}
    void clear() {m_isClear = true; m_type = 0; m_value.clear();}
    bool isClear() const {return m_isClear;}
    operator bool() const {return isClear();}
    QByteArray bigEndianData() const;

private:
    quint8 m_type;
    QByteArray m_value;
    bool m_isClear;

};

Q_DECLARE_METATYPE(FiTLV)

#endif // FITLV_H
