/*
	*
	* Copyright 2014 Britanicus <marcusbritanicus@gmail.com>
	*

	*
	* This program is free software; you can redistribute it and/or modify
	* it under the terms of the GNU General Public License as published by
	* the Free Software Foundation; either version 2 of the License, or
	* (at your option) any later version.
	*

	*
	* This program is distributed in the hope that it will be useful,
	* but WITHOUT ANY WARRANTY; without even the implied warranty of
	* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	* GNU General Public License for more details.
	*

	*
	* You should have received a copy of the GNU General Public License
	* along with this program; if not, write to the Free Software
	* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
	* MA 02110-1301, USA.
	*
*/

// C++ Standard Library
#include <QCoreApplication>

// STL
#include <iostream>
#include <unistd.h>

// LibArchive
#include <libarchiveqt.h>

QString formatSize( qint64 num ) {

	QString total;
	const qint64 kb = 1024;
	const qint64 mb = 1024 * kb;
	const qint64 gb = 1024 * mb;
	const qint64 tb = 1024 * gb;

	if ( num >= tb ) total = QString( "%1 TiB" ).arg( QString::number( qreal( num ) / tb, 'f', 3 ) );
	else if ( num >= gb ) total = QString( "%1 GiB" ).arg( QString::number( qreal( num ) / gb, 'f', 2 ) );
	else if ( num >= mb ) total = QString( "%1 MiB" ).arg( QString::number( qreal( num ) / mb, 'f', 1 ) );
	else if ( num >= kb ) total = QString( "%1 KiB" ).arg( QString::number( qreal( num ) / kb,'f',1 ) );
	else total = QString( "%1 byte%2" ).arg( num ).arg( num > 1 ? "s": "" );

	return total;
};

void printUsage( const char *exec ) {

	std::cout << "Archiver v1.0\n" << std::endl;

	std::cout << "Usage:\n\t" << exec << " -c archive.xxx file1 file2 file3 ..." << std::endl;
	std::cout << "\t" << exec << " -d archive.xxx output_dir" << std::endl;
	std::cout << "\t" << exec << " -m archive.xxx output_dir member_name" << std::endl;
	std::cout << "\t" << exec << " -l archive.xxx" << std::endl;
};

int main( int argc, char** argv ) {

	QCoreApplication app( argc, argv);

	if ( argc < 3 ) {
		printUsage( argv[ 0 ] );
		return 1;
	}

	else if ( ( argc == 3 ) and ( strcmp( argv[ 1 ], "-c" ) == 0 ) ) {
		printUsage( argv[ 0 ] );

		std::cout << "\nArchiver: ERROR: No input files specified." << std::endl;
		return 1;
	}

	else if ( ( argc < 4 ) and ( strcmp( argv[ 1 ], "-m" ) == 0 ) ) {
		printUsage( argv[ 0 ] );

		std::cout << "\nArchiver: ERROR: No member name specified." << std::endl;
		return 1;
	}

	else if ( ( argc > 3 ) and ( strcmp( argv[ 1 ], "-l" ) == 0 ) )  {
		printUsage( argv[ 0 ] );

		std::cout << "\nArchiver: ERROR: Too many files specified" << std::endl;
		return 1;
	}

	else if ( ( argc >= 3 ) and strcmp( argv[ 1 ], "-c" ) and strcmp( argv[ 1 ], "-d" ) and strcmp( argv[ 1 ], "-l" ) and strcmp( argv[ 1 ], "-m" ) ) {

		printUsage( argv[ 0 ] );

		std::cout << "\nArchiver: ERROR: You need to specify one of -c, -d, -m or -l" << std::endl;
		return 1;
	}

	if ( !strcmp( argv[ 1 ], "-c" ) ) {
		// Write archive code
		LibArchive *arc = new LibArchive( argv[ 2 ] );
		arc->updateInputFiles( app.arguments().mid( 3 ) );
		arc->create();
	}

	else if ( !strcmp( argv[ 1 ], "-d" ) ) {
		// Read archive code
		LibArchive *arc = new LibArchive( argv[ 2 ] );
		if ( argc >= 4 )
			arc->setDestination( argv[ 3 ] );
		arc->extract();
	}

	else if ( !strcmp( argv[ 1 ], "-m" ) ) {
		// Read archive code
		LibArchive *arc = new LibArchive( argv[ 2 ] );
		if ( argc == 5 ) {
			arc->setDestination( argv[ 3 ] );
			arc->extractMember( argv[ 4 ] );
		}
		else {
			arc->extractMember( argv[ 3 ] );
		}
	}

	else if ( !strcmp( argv[ 1 ], "-l" ) ) {
		// List archive code
		LibArchive *arc = new LibArchive( argv[ 2 ] );
		qDebug() << arc->list().count();
		Q_FOREACH(  ArchiveEntry *ae, arc->list() ) {
			if ( ae->type == AE_IFREG )
				qDebug() << ae->name.toLocal8Bit().data() << formatSize( ae->size ).toLocal8Bit().data();

			else
				qDebug() << ae->name.toLocal8Bit().data();
		}
	}

	else
		printUsage( argv[ 0 ] );

	return 0;
};
