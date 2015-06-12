#-------------------------------------------------
#
# Project created by QtCreator 2015-04-03T16:02:50
#
#-------------------------------------------------

QT       += core gui\
            network\
            xml\
            serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Gym_Client
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    rfiddialog.cpp \
    notifdialog.cpp \
    settingdialog.cpp \
    caldialog.cpp \
    about.cpp

HEADERS  += mainwindow.h \
    rfiddialog.h \
    notifdialog.h \
    settingdialog.h \
    caldialog.h \
    about.h

FORMS    += mainwindow.ui \
    rfiddialog.ui \
    notifdialog.ui \
    settingdialog.ui \
    caldialog.ui \
    about.ui
