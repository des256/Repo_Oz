#-------------------------------------------------
#
# Project created by QtCreator 2014-02-21T18:31:40
#
#-------------------------------------------------

QT       += core gui network widgets multimedia multimediawidgets

TARGET = OzTool
TEMPLATE = app

win32: DESTDIR = ../outputs/win32-i686
unix:!macx: DESTDIR = ../outputs/linux-i686

SOURCES += main.cpp\
    MainWindow.cpp \
    ConnectDialog.cpp \
    Model.cpp \
    Node.cpp \
    Transition.cpp \
    NodeDialog.cpp \
    NewNodeCommand.cpp \
    DeleteNodeCommand.cpp \
    EditNodeCommand.cpp \
    MoveNodeCommand.cpp \
    TransitionDialog.cpp \
    DeleteTransitionCommand.cpp \
    EditTransitionCommand.cpp \
    MoveTransitionCommand.cpp \
    NewTransitionCommand.cpp \
    ConnectTransitionCommand.cpp \
    ../Client1.cpp

HEADERS  += \
    MainWindow.h \
    ConnectDialog.h \
    Model.h \
    Node.h \
    Transition.h \
    NodeDialog.h \
    Command.h \
    NewNodeCommand.h \
    DeleteNodeCommand.h \
    EditNodeCommand.h \
    MoveNodeCommand.h \
    TransitionDialog.h \
    DeleteTransitionCommand.h \
    EditTransitionCommand.h \
    MoveTransitionCommand.h \
    NewTransitionCommand.h \
    ConnectTransitionCommand.h \
    ../Client1.h \
    ../protocol.h

win32:LIBS += -lws2_32

OTHER_FILES += \
    details.txt
