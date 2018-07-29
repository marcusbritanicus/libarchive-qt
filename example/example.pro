TEMPLATE = app
TARGET = archiver

DEPENDPATH += . ../lib/
INCLUDEPATH += . ../lib/

LIBS += -larchive -lz -lbz2 -llzma

# Input
SOURCES += example.cpp

# Input
HEADERS += ../lib/libarchiveqt.h ../lib/LibLzma.hpp ../lib/LibLzma2.hpp ../lib/LibBZip2.hpp ../lib/LibGZip.hpp
SOURCES += ../lib/LibArchive.cpp ../lib/LibLzma.cpp ../lib/LibLzma2.cpp ../lib/LibBZip2.cpp ../lib/LibGZip.cpp

CONFIG += silent warn_on
QT -= gui

MOC_DIR 	= ../build/moc
OBJECTS_DIR = ../build/obj
RCC_DIR		= ../build/qrc
UI_DIR      = ../build/uic
