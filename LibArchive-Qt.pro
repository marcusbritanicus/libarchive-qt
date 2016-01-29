TEMPLATE = app
TARGET = archiver

DEPENDPATH += . MimeHandler StandardPaths
INCLUDEPATH += . MimeHandler StandardPaths

LIBS += -larchive -lz -lbz2 -llzma

# Input
HEADERS += LibArchive.hpp LibLzma.hpp LibBZip2.hpp LibGZip.hpp
SOURCES += LibArchive.cpp LibLzma.cpp LibBZip2.cpp LibGZip.cpp Main.cpp

# MimeType handling + QStandardPaths
lessThan(QT_MAJOR_VERSION, 5) {
	HEADERS += MimeHandler/*.hpp StandardPaths/*.hpp
	SOURCES += MimeHandler/*.cpp StandardPaths/*.cpp
}

MOC_DIR 	= build/moc
OBJECTS_DIR = build/objs
RCC_DIR		= build/qrc
UI_DIR      = build/uic

CONFIG += silent warn_on
QT -= gui
