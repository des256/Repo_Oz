#-------------------------------------------------
#
# Project created by QtCreator 2014-02-21T18:31:40
#
#-------------------------------------------------

QT       += core gui network widgets multimedia multimediawidgets

TARGET = OzRemote
TEMPLATE = app
CONFIG += console static

win32: DESTDIR = ../outputs/win32-i686/bin
unix:!macx: DESTDIR = ../outputs/linux-i686/bin

SOURCES += main.cpp\
    MainWindow.cpp \
    ConnectDialog.cpp \
    QClient1.cpp

HEADERS  += \
    MainWindow.h \
    ConnectDialog.h \
    ../protocol.h \
    QClient1.h

win32: LIBS += -L$$PWD/../outputs/win32-i686/lib -lavcodec -lavformat -lavutil -lswscale
else:unix:!macx: LIBS += -lavcodec -lavformat -lavutil -lswscale
win32: INCLUDEPATH += $$PWD/../outputs/include

win32: LIBS += -lws2_32

OTHER_FILES +=
