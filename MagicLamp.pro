#-------------------------------------------------
#
# Project created by QtCreator 2015-02-08T20:37:38
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MagicLamp
TEMPLATE = app


SOURCES += main.cpp\
    fitlv.cpp \
    fitlvstream.cpp \
    dialog.cpp \
    filamp.cpp \
    fimagiccommand.cpp \
    ipedit.cpp \
    fitcpsocketforthread.cpp

HEADERS  += \
    fitlv.h \
    fitlvstream.h \
    dialog.h \
    filamp.h \
    fimagiccommand.h \
    ipedit.h \
    fitcpsocketforthread.h

OTHER_FILES +=

FORMS += \
    dialog.ui
