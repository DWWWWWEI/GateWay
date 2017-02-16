QT += core network

QT += core gui network serialbus

QT -= gui

CONFIG += c++11

TARGET = GW
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    gwengine.cpp \
    connectionmanager.cpp \
    servercommhelper.cpp \
    ibusinesshandler.cpp \
    businesshandlerfactory.cpp \
    devicemanager.cpp \
    devicecontrollercom.cpp \
    iprotocolhandler.cpp \
    modbushandler.cpp \
    protocolhandlerfactory.cpp \
    icommunicationcarrier.cpp \
    rs485carrier.cpp \
    communicationcarrierfactory.cpp \
    singleton.cpp \
    watersupplyhandler.cpp \
    uuidgenerator.cpp \
    test.cpp

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
    gwengine.h \
    uuidgenerator.h \
    connectionmanager.h \
    servercommhelper.h \
    ibusinesshandler.h \
    businesshandlerfactory.h \
    devicemanager.h \
    devicecontrollercom.h \
    iprotocolhandler.h \
    modbushandler.h \
    protocolhandlerfactory.h \
    icommunicationcarrier.h \
    rs485carrier.h \
    communicationcarrierfactory.h \
    singleton.h \
    watersupplyhandler.h \
    commontypesdef.h \
    test.h

DISTFILES +=
