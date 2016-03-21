TEMPLATE = lib
TARGET = archiveqt

DEPENDPATH += . MimeHandler StandardPaths
INCLUDEPATH += . MimeHandler StandardPaths

LIBS += -larchive -lz -lbz2 -llzma

# Input
HEADERS += LibArchive.hpp LibLzma.hpp LibBZip2.hpp LibGZip.hpp
SOURCES += LibArchive.cpp LibLzma.cpp LibBZip2.cpp LibGZip.cpp

# MimeType handling + QStandardPaths
lessThan(QT_MAJOR_VERSION, 5) {
	HEADERS += MimeHandler/*.hpp StandardPaths/*.hpp
	SOURCES += MimeHandler/*.cpp StandardPaths/*.cpp
}

CONFIG += silent warn_on
QT -= gui

MOC_DIR 	= ../build/moc-X
OBJECTS_DIR = ../build/obj-X
RCC_DIR		= ../build/qrc-X
UI_DIR      = ../build/uic-X
