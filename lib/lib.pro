TEMPLATE = lib
TARGET = archiveqt

greaterThan(QT_MAJOR_VERSION, 4) {
	TARGET = archiveqt5
}

DEPENDPATH += . MimeHandler StandardPaths
INCLUDEPATH += . MimeHandler StandardPaths

LIBS += -larchive -lz -lbz2 -llzma

# Input
HEADERS += libarchiveqt.h LibLzma.hpp LibLzma2.hpp LibBZip2.hpp LibGZip.hpp
SOURCES += LibArchive.cpp LibLzma.cpp LibLzma2.cpp LibBZip2.cpp LibGZip.cpp

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

unix {
	isEmpty(PREFIX) {
		PREFIX = /usr
	}

	INSTALLS += target includes

	target.path = $$PREFIX/lib/

	includes.path = $$PREFIX/include/
	includes.files = libarchive.h

	data.path = $$PREFIX/share/libarchiveqt/
	data.files = README Changelog ReleaseNotes
}
