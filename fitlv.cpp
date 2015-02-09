#include "fitlv.h"

#include <QtCore/QtEndian>

FiTLV::FiTLV()
   : m_type(0)
{
}

void FiTLV::setType(quint8 type)
{
    m_type = type;
    m_value.clear();
}

QByteArray FiTLV::bigEndianData() const
{
    QByteArray result;
    result.push_back(m_type);
    uchar length[2] = {0 ,0};
    qToBigEndian((quint16)m_value.size(), length);
    result.push_back(length[0]);
    result.push_back(length[1]);
    result.push_back(m_value);
    return result;
}
