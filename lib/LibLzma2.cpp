/**
 * Copyright 2018 Britanicus <marcusbritanicus@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **/

// Local Headers
#include <LibLzma2.hpp>

const int MAX_READ_SIZE = 40960;

QString NBXz::xzFileName = QString();
QString NBXz::fileName   = QString();

NBXz::NBXz( QString archive, QString file ) {
    xzFileName = QString( archive );

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

    else {
        fileName = file;
        fileName.chop( 3 );
    }

    fdin  = fopen( qPrintable( xzFileName ), "rb" );
    fdout = fopen( qPrintable( fileName ), "wb" );
}


bool NBXz::extract() {
    lzma_stream strm = LZMA_STREAM_INIT;
    lzma_ret    ret;

    // Initialize the decoder
    ret = lzma_stream_decoder( &strm, UINT64_MAX, LZMA_CONCATENATED );

    if ( ret != LZMA_OK ) {
        return false;
    }

    uint8_t in_buf[ MAX_READ_SIZE ];
    uint8_t out_buf[ MAX_READ_SIZE ];

    strm.avail_in  = 0;
    strm.next_out  = out_buf;
    strm.avail_out = MAX_READ_SIZE;

    lzma_action action = LZMA_RUN;

    while ( true ) {
        if ( strm.avail_in == 0 ) {
            strm.next_in  = in_buf;
            strm.avail_in = fread( in_buf, 1, MAX_READ_SIZE, fdin );

            if ( ferror( fdin ) ) {
                return false;
            }
        }

        if ( feof( fdin ) ) {
            action = LZMA_FINISH;
        }

        ret = lzma_code( &strm, action );

        if ( (strm.avail_out == 0) || (ret != LZMA_OK) ) {
            const size_t write_size = MAX_READ_SIZE - strm.avail_out;

            if ( fwrite( out_buf, 1, write_size, fdout ) != write_size ) {
                return false;
            }

            strm.next_out  = out_buf;
            strm.avail_out = MAX_READ_SIZE;
        }

        if ( ret != LZMA_OK ) {
            if ( ret == LZMA_STREAM_END ) {
                // lzma_stream_decoder() already guarantees that there's no trailing garbage.
                assert( strm.avail_in == 0 );
                assert( action == LZMA_FINISH );
                assert( feof( fdin ) );
                return false;
            }
        }
    }

    return true;
}
