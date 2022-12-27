#-------------------------------------------------
#
# Project created by QtCreator 2013-04-18T15:05:07
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QtAwesomeSample
TEMPLATE = app

SOURCES += main.cpp \
    mainwindow.cpp

HEADERS  += \
    mainwindow.h


# only one option must be enabled
#CONFIG += fontAwesomePro
CONFIG += fontAwesomeFree

include(../QtAwesome/QtAwesome.pri)

FORMS += \
    mainwindow.ui
