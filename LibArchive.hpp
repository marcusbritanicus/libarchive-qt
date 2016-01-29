/*
	*
	* LibArchive.hpp - LibArchive.cpp header
	*
*/

#pragma once

#include <Global.hpp>

#include <LibLzma.hpp>
#include <LibBZip2.hpp>
#include <LibGZip.hpp>

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

	private:
		int copyData( struct archive *ar, struct archive *aw );
		int setFilterFormat( struct archive *ar, QMimeType mType );

		QString archiveName;

		QStringList inputList;
		QString dest;
		QString src;
};
