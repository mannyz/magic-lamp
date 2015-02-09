#include "fimagiccommand.h"

FiMagicCommand::FiMagicCommand()
    : m_type(FiMagicCommand::UNKNOWN)
{
}

FiMagicCommand::FiMagicCommand(Type type)
    : m_type(type)
{

}


FiCommandOn::FiCommandOn()
    : FiMagicCommand(FiMagicCommand::ON)
{

}

FiTLV FiCommandOn::tlvFromData() const
{
    FiTLV result;
    result.setType(FiMagicCommand::ON);
    return result;
}

FiCommandOff::FiCommandOff()
    : FiMagicCommand(FiMagicCommand::OFF)
{

}

FiTLV FiCommandOff::tlvFromData() const
{
    FiTLV result;
    result.setType(FiMagicCommand::OFF);
    return result;
}

FiCommandColor::FiCommandColor()
    : FiMagicCommand(FiMagicCommand::COLOR)
{

}

FiTLV FiCommandColor::tlvFromData() const
{
    FiTLV result;
    result.setType(FiMagicCommand::COLOR);
    QByteArray value;
    value.push_back(m_color.red());
    value.push_back(m_color.green());
    value.push_back(m_color.blue());
    result.setValue(value);
    return result;
}

void FiCommandColor::setDataFromTLV(const FiTLV &tlv)
{
    m_color.setRgb((uchar)tlv.value()[0], (uchar)tlv.value()[1], (uchar)tlv.value()[2]);
}
