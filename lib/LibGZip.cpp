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

// Local Headers
#include <LibGZip.hpp>

const int MAX_READ_SIZE = 40960;

QString NBGZip::gzFileName = QString();
QString NBGZip::fileName = QString();

NBGZip::NBGZip( QString archive, QString file ) {

	gzFileName = QString( archive );
	if ( not file.isEmpty() ) {
		if ( QFileInfo( file ).isDir() ) {
			fileName = QDir( file ).filePath( QString( archive ) );
			fileName.chop( 3 );
		}

		else if ( QFileInfo( file ).exists() ) {
			QFile::rename( file, file + ".old" );
			fileName = QString( file );
		}

		else {
			fileName = QString( file );
		}
	}

	gzip = gzopen( qPrintable( gzFileName ), "rb" );
};

bool NBGZip::extract() {

	if ( gzip == NULL )
		return true;

	// Reading from the bz2 file opened
	std::ofstream ofile( qPrintable( fileName ), std::ofstream::binary );

	while ( true ) {
		char buffer[ MAX_READ_SIZE ] = { "\x00" };
		int charsRead = gzread( gzip, buffer, sizeof( buffer ) );

		if ( charsRead > 0 )
			ofile.write( buffer, charsRead );

		else if ( charsRead == 0 )
			break;

		else
			return true;
	}

	// Close the file
	ofile.close();
	if ( gzclose( gzip ) != Z_OK )
		return true;

	return true;
};
