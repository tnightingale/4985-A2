#-------------------------------------------------
#
# Project created by QtCreator 2011-02-11T18:16:10
#
#-------------------------------------------------

QT       += core gui network

TARGET = 4985-A2
TEMPLATE = app

LIBS += C:\Qt\2010.05\mingw\lib\libws2_32.a

SOURCES += main.cpp\
        mainwindow.cpp \
    server.cpp \
    client.cpp \
    socket.cpp \
    tcpsocket.cpp \
    udpsocket.cpp

HEADERS  += mainwindow.h \
    server.h \
    client.h \
    socket.h \
    tcpsocket.h \
    udpsocket.h

FORMS    += mainwindow.ui
