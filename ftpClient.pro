#-------------------------------------------------
#
# Project created by QtCreator 2020-2-17T19:48:16
#
#-------------------------------------------------

QT       += core gui

LIBS += -lpthread libwsock32 libws2_32

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ftpClient
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        main.cpp \
        ftpclient.cpp \
    client.cpp \
    clientthread.cpp \
    infothread.cpp

HEADERS += \
        ftpclient.h\
    client.h \
    clientthread.h \
    infothread.h


FORMS += \
        ftpclient.ui

