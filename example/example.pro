TEMPLATE = app
TARGET = archiver

DEPENDPATH += .
INCLUDEPATH += . ../lib/

LIBS += -L../lib/ -larchiveqt

# Input
SOURCES += example.cpp

CONFIG += silent warn_on
QT -= gui

MOC_DIR 	= ../build/moc
OBJECTS_DIR = ../build/obj
RCC_DIR		= ../build/qrc
UI_DIR      = ../build/uic
