TEMPLATE = app
TARGET = archiver

DEPENDPATH += . ../lib/
INCLUDEPATH += . ../lib/

LIBS += ../lib/libarchiveqt5.a -larchive -lz -lbz2 -llzma

# Input
SOURCES += example.cpp

CONFIG += silent warn_on
QT -= gui

MOC_DIR 	= ../build/moc
OBJECTS_DIR = ../build/obj
RCC_DIR		= ../build/qrc
UI_DIR      = ../build/uic
