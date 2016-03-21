/*
	*
	* LibArchive.hpp - LibArchive.cpp header
	*
*/

#pragma once

// LibArchive
#include <archive.h>
#include <archive_entry.h>

// Qt Headers
#include <QtCore>

// SystemWide Headers
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>

#include <LibLzma.hpp>
#include <LibLzma2.hpp>
#include <LibBZip2.hpp>
#include <LibGZip.hpp>

class QMimeType;

typedef struct {

	/* Name of the entry */
	QString name;

	/* Size of the entry */
	quint64 size;

	/* Type of the entry */
	int type;

	/* Stat equivalent */
	struct stat *stat;

} ArchiveEntry;

typedef QList<ArchiveEntry*> ArchiveEntries;

class LibArchive {

	public:
		LibArchive( QString );

		// Convinience Functions
		void updateInputFiles( QStringList );
		void setWorkingDir( QString );
		void setDestination( QString );

		// Workers
		void create();
		int extract();
		ArchiveEntries list();

	private:
		int copyData( struct archive *ar, struct archive *aw );
		int setFilterFormat( struct archive *ar, QMimeType mType );

		QString archiveName;

		QStringList inputList;
		QString dest;
		QString src;
};
