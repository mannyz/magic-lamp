#ifndef FITLV_H
#define FITLV_H

#include <QtCore/QtGlobal>
#include <QtCore/QByteArray>

class FiTLV {
public:
    FiTLV();

    quint8 type() const {return m_type;}
    void setType(quint8 type);

    quint16 length() const {return m_value.size();}
    void setLength(quint16 length) {m_value.resize(length);}

    const QByteArray &value() const {return m_value;}
    void setValue(const QByteArray &value) {m_value = value;}

    QByteArray bigEndianData() const;

private:
    quint8 m_type;
    QByteArray m_value;

};

#endif // FITLV_H
