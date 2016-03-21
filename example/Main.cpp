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
#include <LibArchive.hpp>

void printUsage( const char *exec ) {

	std::cout << "Archiver v1.0\n" << std::endl;

	std::cout << "Usage:\n\t" << exec << " -c archive.xxx file1 file2 file3 ..." << std::endl;
	std::cout << "\t" << exec << " -d archive.xxx output_dir" << std::endl;
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

	else if ( ( argc > 3 ) and ( strcmp( argv[ 1 ], "-l" ) == 0 ) )  {
		printUsage( argv[ 0 ] );

		std::cout << "\nArchiver: ERROR: Too many files specified" << std::endl;
		return 1;
	}

	else if ( ( argc >= 3 ) and strcmp( argv[ 1 ], "-c" ) and strcmp( argv[ 1 ], "-d" ) and strcmp( argv[ 1 ], "-l" ) ) {

		printUsage( argv[ 0 ] );

		std::cout << "\nArchiver: ERROR: You need to specify one of -c, -d or -l" << std::endl;
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

	else if ( !strcmp( argv[ 1 ], "-l" ) ) {
		// List archive code
		LibArchive *arc = new LibArchive( argv[ 2 ] );
		arc->list();
	}



	else
		printUsage( argv[ 0 ] );

	return 0;
};
