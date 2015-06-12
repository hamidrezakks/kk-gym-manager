#-------------------------------------------------
#
# Project created by QtCreator 2015-04-02T12:17:23
#
#-------------------------------------------------

QT       += core gui\
            network\
            sql\
            multimedia\
            multimediawidgets\
            serialport\
            xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Gym_Manager
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    rfiddialog.cpp \
    addmemberdialog.cpp \
    netsettingdialog.cpp \
    rfidsettingdialog.cpp \
    about.cpp

HEADERS  += mainwindow.h \
    rfiddialog.h \
    addmemberdialog.h \
    netsettingdialog.h \
    rfidsettingdialog.h \
    about.h

FORMS    += mainwindow.ui \
    rfiddialog.ui \
    addmemberdialog.ui \
    netsettingdialog.ui \
    rfidsettingdialog.ui \
    about.ui
