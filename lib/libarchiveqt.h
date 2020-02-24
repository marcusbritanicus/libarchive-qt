/*
	*
	* Copyright 2018 Britanicus <marcusbritanicus@gmail.com>
	*

	*
	* This program is free software: you can redistribute it and/or modify
	* it under the terms of the GNU Lesser General Public License as published by
	* the Free Software Foundation, either version 3 of the License, or
	* (at your option) any later version.
	*

	*
	* This program is distributed in the hope that it will be useful,
	* but WITHOUT ANY WARRANTY; without even the implied warranty of
	* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	* GNU Lesser General Public License for more details.
	*

	*
	* You should have received a copy of the GNU Lesser General Public License
	* along with this program.  If not, see <http://www.gnu.org/licenses/>.
	*
*/

#pragma once

// LibArchive
#include <archive.h>
#include <archive_entry.h>

// Qt Headers
#include <QtCore>

class QMimeType;

typedef struct {

	/* Name of the entry */
	QString name;

	/* Size of the entry */
	quint64 size;

	/* Type of the entry */
	int type;

	/* Stat equivalent */
	struct stat info;
} ArchiveEntry;

typedef QList<ArchiveEntry*> ArchiveEntries;

class LibArchiveQt : public QThread {
	Q_OBJECT

	public:
		LibArchiveQt( QString );

		// Convinience Functions
		void updateInputFiles( QStringList );
		void setWorkingDir( QString );
		void setDestination( QString );
		void waitForFinished();

		/* Create an archive */
		void createArchive();

		/* Extract the archive */
		void extractArchive();

		/* Extract a named member of the archive */
		void extractMember( QString );

		/* List the contetns of the archive */
		ArchiveEntries listArchive();

		/* Exit status */
		int exitStatus();

	private:
		enum Mode {
			None				= 0xF650E7,
			Single,
			Container
		};

		enum Job {
			NoJob				= 0x25CEE9,
			CreateArchive,
			ExtractArchive,
			ExtractMember,
			ListArchive
		};

		/* Internal worker for copying data */
		int copyData( struct archive *ar, struct archive *aw );

		/* Set the archive filter format based on extensions */
		void setFilterFormat( QMimeType mType );

		/* Create an archive - Internal Worker */
		bool doCreateArchive();

		/* Extract the archive - Internal Worker */
		bool doExtractArchive();

		/* Extract a named member of the archive - Internal Worker */
		bool doExtractMember( QString );

		int mArchiveFilter;
		int mArchiveFormat;

		QString archiveName;

		QStringList inputList;
		QString dest;
		QString src;

		ArchiveEntries memberList;
		bool readDone;
		int archiveType;

		/* What job are we doing? */
		int mJob;

		/* Is the job running? */
		bool isRunning;

		/* Exit status */
		int mExitStatus;

		/* Member to be extracted */
		QString extractedMember;

	protected:
		void run();

	Q_SIGNALS:
		void jobComplete();
		void jobFailed();
};
