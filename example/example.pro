TEMPLATE = app
TARGET = archiver

DEPENDPATH += .
INCLUDEPATH += . ../lib/

LIBS += -L../lib/ -larchiveqt

# Input
SOURCES += Main.cpp

CONFIG += silent warn_on
QT -= gui

MOC_DIR 	= ../build/moc-X
OBJECTS_DIR = ../build/obj-X
RCC_DIR		= ../build/qrc-X
UI_DIR      = ../build/uic-X
