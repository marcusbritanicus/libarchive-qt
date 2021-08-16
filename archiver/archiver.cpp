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

// C++ Standard Library
#include <QCoreApplication>

// STL
#include <iostream>
#include <unistd.h>

// LibArchive
#include "libarchiveqt.h"

/* To pretty print file sizes */
QString formatSize( qint64 num ) {

	QString total;
	const qint64 kb = 1024;
	const qint64 mb = 1024 * kb;
	const qint64 gb = 1024 * mb;
	const qint64 tb = 1024 * gb;

	if ( num >= tb )      total = QString( "%1 TiB" ).arg( QString::number( qreal( num ) / tb, 'f', 3 ) );
	else if ( num >= gb ) total = QString( "%1 GiB" ).arg( QString::number( qreal( num ) / gb, 'f', 2 ) );
	else if ( num >= mb ) total = QString( "%1 MiB" ).arg( QString::number( qreal( num ) / mb, 'f', 1 ) );
	else if ( num >= kb ) total = QString( "%1 KiB" ).arg( QString::number( qreal( num ) / kb,'f',1 ) );
	else                  total = QString( "%1 byte%2" ).arg( num ).arg( num > 1 ? "s": "" );

	return total;
};

/* Pretty print the usage */
void printUsage( const char *exec ) {

	std::cout << "Archiver v" ARCHIVEQT_VERSION_STR "\n" << std::endl;

	std::cout << "Usage:\n\t" << exec << " -c archive.xxx file1 file2 file3 ..." << std::endl;
	std::cout << "\t" << exec << " -d archive.xxx output_dir" << std::endl;
	std::cout << "\t" << exec << " -m archive.xxx member_name output_dir" << std::endl;
	std::cout << "\t" << exec << " -l archive.xxx" << std::endl;
};

int main( int argc, char** argv ) {

	/* Init the QApplication instance */
	QCoreApplication app( argc, argv);

	/*
		*
		* We need three arguments at minimum
		* 	1. The program name (argv[ 0 ]) => Always existing
		* 	2. The switch: one of c, d, m or l: Tells the program what to do
		* 	3. The archive name to operate on (list or decompress)
		*	In case we are compressing or extracting a member, an additional argument is necessary
		*
	*/
	if ( argc < 3 ) {

		printUsage( argv[ 0 ] );
		return 1;
	}

	/* Print help text or usage */
	else if ( ( argc == 2 ) and ( strcmp( argv[ 1 ], "-h" ) == 0 ) ) {

		printUsage( argv[ 0 ] );
		return 0;
	}

	/* Print help text or usage */
	else if ( ( argc == 2 ) and ( strcmp( argv[ 1 ], "--help" ) == 0 ) ) {

		printUsage( argv[ 0 ] );
		return 0;
	}

	/* Switch c, but no input files mentioned */
	else if ( ( argc == 3 ) and ( strcmp( argv[ 1 ], "-c" ) == 0 ) ) {

		printUsage( argv[ 0 ] );

		std::cout << "\nArchiver: ERROR: No input files specified." << std::endl;
		return 1;
	}

	/* Switch m, but no member name mentioned */
	else if ( ( argc < 4 ) and ( strcmp( argv[ 1 ], "-m" ) == 0 ) ) {

		printUsage( argv[ 0 ] );

		std::cout << "\nArchiver: ERROR: No member name specified." << std::endl;
		return 1;
	}

	/* Excess arguments */
	else if ( ( argc > 3 ) and ( strcmp( argv[ 1 ], "-l" ) == 0 ) )  {

		printUsage( argv[ 0 ] );

		std::cout << "\nArchiver: ERROR: Too many files specified" << std::endl;
		return 1;
	}

	/* No switch mentioned */
	else if ( ( argc >= 3 ) and strcmp( argv[ 1 ], "-c" ) and strcmp( argv[ 1 ], "-d" ) and strcmp( argv[ 1 ], "-l" ) and strcmp( argv[ 1 ], "-m" ) ) {

		printUsage( argv[ 0 ] );

		std::cout << "\nArchiver: ERROR: You need to specify one of -c, -d, -m or -l" << std::endl;
		return 1;
	}

	/* Compress the input files argv[3+] into archive argv[2] */
	else if ( strcmp( argv[ 1 ], "-c" ) == 0 ) {

		// Write archive code
		LibArchiveQt *arc = new LibArchiveQt( argv[ 2 ] );

		arc->updateInputFiles( app.arguments().mid( 3 ), LibArchiveQt::RelativeToCurrent );
		arc->createArchive();
		arc->waitForFinished();

		return 0;
	}

	/* Decompress the archive argv[2] optionally to argv[3] */
	else if ( strcmp( argv[ 1 ], "-d" ) == 0 ) {

		// Read archive code
		LibArchiveQt *arc = new LibArchiveQt( argv[ 2 ] );

		if ( argc >= 4 )
			arc->setDestination( argv[ 3 ] );

		arc->extractArchive();
		arc->waitForFinished();

		return 0;
	}

	/* Decompress the member argv[3] optionally to argv[4] from archive argv[2] */
	else if ( strcmp( argv[ 1 ], "-m" ) == 0 ) {

		// Read archive code
		LibArchiveQt *arc = new LibArchiveQt( argv[ 2 ] );

		if ( argc == 5 ) {
			arc->setDestination( argv[ 3 ] );
			arc->extractMember( argv[ 4 ] );
		}
		else {
			arc->extractMember( argv[ 3 ] );
		}

		arc->waitForFinished();
		return 0;
	}

	/* List archive argv[2] */
	else if ( strcmp( argv[ 1 ], "-l" ) == 0 ) {

		// List archive code
		LibArchiveQt *arc = new LibArchiveQt( argv[ 2 ] );

		int length = 0;
		Q_FOREACH( ArchiveEntry *ae, arc->listArchive() )
			length = ( ae->name.length() > length ? ae->name.length() : length );

		Q_FOREACH( ArchiveEntry *ae, arc->listArchive() ) {
			if ( ae->type == AE_IFREG )
				qDebug() << ae->name.rightJustified( length + 10 ).toLocal8Bit().data() << "  " << formatSize( ae->size ).toLocal8Bit().data();

			else
				qDebug() << ae->name.rightJustified( length + 10 ).toLocal8Bit().data();
		}

		arc->waitForFinished();

		return 0;
	}

	/* Print help text */
	else {

		printUsage( argv[ 0 ] );
		return 0;
	}
};
