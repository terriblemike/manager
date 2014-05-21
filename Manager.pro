#-------------------------------------------------
#
# Project created by QtCreator 2014-02-10T20:51:18
#
#-------------------------------------------------

QT       += core gui serialport declarative network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = manager
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    pserialport.cpp \
    pencoder.cpp \
    pdalnomer.cpp \
    pbmsd.cpp \
    pbmsdport.cpp \
    ppotenciometr.cpp \
    ppult.cpp \
    pstaticdio.cpp \
    pjoystick.cpp \
    network.cpp \
    pdrive.cpp \
    pgsm40.cpp \
    ptest.cpp \
    parser.cpp \
    anetwork.cpp

HEADERS  += mainwindow.h \
    pserialport.h \
    pencoder.h \
    pdalnomer.h \
    pbmsd.h \
    pbmsdport.h \
    ppotenciometr.h \
    ppult.h \
    CP_Button_Definitions.h \
    pstaticdio.h \
    pjoystick.h \
    network.h \
    netDefine.h \
    pdrive.h \
    pgsm40.h \
    ptest.h \
    pstatus.h \
    parser.h \
    anetwork.h

FORMS    += mainwindow.ui

LIBS += -lSDL

OTHER_FILES += \
    command_codes.txt \
    settings.ini \
    Automation.BDaq.jar \
    enccalib.ini \
    Zametki.txt \
    Works.txt \
    Questions.txt

RESOURCES += \
    images.qrc
