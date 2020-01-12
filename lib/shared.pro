TEMPLATE	= lib
TARGET		= archiveqt5

lessThan( QT_MAJOR_VERSION, 5 ) {
	TARGET = archiveqt
}

VERSION   = 1.1.0

DEPENDPATH 	+= . MimeHandler StandardPaths
INCLUDEPATH	+= . MimeHandler StandardPaths

LIBS += -larchive -lz -lbz2 -llzma

# Sources
HEADERS += libarchiveqt.h LibLzma.hpp LibLzma2.hpp LibBZip2.hpp LibGZip.hpp
SOURCES += LibArchive.cpp LibLzma.cpp LibLzma2.cpp LibBZip2.cpp LibGZip.cpp

# LZ4
HEADERS += lz4dec.h
SOURCES += lz4dec.c

contains ( DEFINES, HAVE_LZLIB ) {
	LIBS += -llz

	HEADERS += LibLZip.hpp
	SOURCES += LibLZip.cpp
}

# MimeType handling + QStandardPaths
lessThan(QT_MAJOR_VERSION, 5) {
	HEADERS += MimeHandler.hpp StandardPaths.hpp
	SOURCES += MimeHandler.cpp StandardPaths.cpp
}

CONFIG += silent warn_on shared
QT -= gui static

MOC_DIR			= ../build/moc
OBJECTS_DIR		= ../build/obj
RCC_DIR			= ../build/qrc
UI_DIR			= ../build/uic

unix {
	isEmpty(PREFIX) {
		PREFIX = /usr
	}

	INSTALLS	+= target includes data
	CONFIG		+= no_install_prl
	contains(DEFINES, LIB64): target.path = $$INSTALL_PREFIX/lib64
	else: target.path = $$INSTALL_PREFIX/lib

	target.path			= $$PREFIX/lib/
	includes.files		= libarchiveqt.h
	includes.path		= $$PREFIX/include/

	data.path = $$PREFIX/share/lib$$TARGET/
	data.files = Changelog README
}

macx {
	INCLUDEPATH += /usr/local/opt/libarchive/include /usr/local/opt/xz/include
	QMAKE_LFLAGS += -L/usr/local/opt/libarchive/lib -L/usr/local/opt/xz/lib
}
