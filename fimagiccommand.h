#ifndef FIMAGICCOMMAND_H
#define FIMAGICCOMMAND_H

#include "fitlv.h"

#include <QtGui/QColor>

class FiMagicCommand
{

public:
    enum Type {
        UNKNOWN = 0xFF,
        ON = 0x12,
        OFF = 0x13,
        COLOR = 0x20
    };

public:
    FiMagicCommand();
    FiMagicCommand(Type type);
    virtual ~FiMagicCommand() {}

    Type type() const {return m_type;}
    void setType(Type type) {m_type = type;}

    virtual FiTLV tlvFromData() const =0;
    virtual void setDataFromTLV(const FiTLV &tlv) =0;

private:
    Type m_type;

};

class FiCommandOn: public FiMagicCommand {
public:
    FiCommandOn();

    virtual FiTLV tlvFromData() const;
    virtual void setDataFromTLV(const FiTLV &) {}

};

class FiCommandOff: public FiMagicCommand {
public:
    FiCommandOff();

    virtual FiTLV tlvFromData() const;
    virtual void setDataFromTLV(const FiTLV &) {}

};

class FiCommandColor: public FiMagicCommand {
public:
    FiCommandColor();

    virtual FiTLV tlvFromData() const;
    virtual void setDataFromTLV(const FiTLV &tlv);

    void setColor(const QColor &color) {m_color = color;}
    const QColor &color() {return m_color;}

private:
    QColor m_color;

};

#endif // FIMAGICCOMMAND_H
