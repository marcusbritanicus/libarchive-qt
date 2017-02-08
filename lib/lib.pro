TEMPLATE	= lib
TARGET		= archiveqt

greaterThan(QT_MAJOR_VERSION, 4) {
	TARGET = archiveqt5
}

VERSION   = 1.0.0

DEPENDPATH 	+= . MimeHandler StandardPaths
INCLUDEPATH	+= . MimeHandler StandardPaths

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

MOC_DIR			= ../build/moc-X
OBJECTS_DIR	= ../build/obj-X
RCC_DIR			= ../build/qrc-X
UI_DIR			= ../build/uic-X

unix {
	isEmpty(PREFIX) {
		PREFIX = /usr
	}

	INSTALLS	+= target includes data
	CONFIG		+= create_pc create_prl no_install_prl link_pkgconfig
	contains(DEFINES, LIB64): target.path = $$INSTALL_PREFIX/lib64
	else: target.path = $$INSTALL_PREFIX/lib

	target.path			= $$PREFIX/lib/
	includes.files	= libarchiveqt.h
	includes.path		= $$PREFIX/include/lib$$TARGET

	data.path = $$PREFIX/share/lib$$TARGET/
	data.files = Changelog README

	QMAKE_PKGCONFIG_NAME = libarchive-qt
	QMAKE_PKGCONFIG_DESCRIPTION = A Qt based archiving solution with libarchive backend
	QMAKE_PKGCONFIG_PREFIX  = $$INSTALL_PREFIX
	QMAKE_PKGCONFIG_LIBDIR  = $$target.path
	QMAKE_PKGCONFIG_INCDIR  = $$includes.path
	QMAKE_PKGCONFIG_VERSION = $$VERSION
	QMAKE_PKGCONFIG_DESTDIR = pkgconfig
}
