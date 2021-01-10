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
		/* Used with updateInputFiles, this helps libarchive to choose how to handle paths */
		enum InputFileMode {
			AbsolutePath = 0x703857,	// Use absolute file paths - Discouraged
			RelativeToRoot,				// All paths will be relative to '/' - Useful for packaging of installed files
			RelativeToHome,				// Files will have paths relative to home folder - good for saving config files
			RelativeToCurrent,			// The paths will be relative to the current path - Useful for archiving files in current dir [Default]
			RelativeToWorkDir,			// Set archive paths of the file relative to @src - @src should be set before calling updateInputFiles
			CommonRelativePath,			// Added files will have paths relative to path common to all files - Costly for large number of files
		};

		LibArchiveQt( QString );

		// Convenience Functions
		void updateInputFiles( QStringList, LibArchiveQt::InputFileMode inMode = LibArchiveQt::RelativeToCurrent );
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

		/* Convenience function */
		static QString suffix( QString name );

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

		QHash<QString, QString> inputList;
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

		/* Progress is always in percentage */
		void progress( int );
};
