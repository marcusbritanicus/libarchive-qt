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

#include "Global.hpp"
#include "libarchiveqt.h"

#include "LibLzma.hpp"
#include "LibLzma2.hpp"
#include "LibBZip2.hpp"
#include "LibGZip.hpp"

extern "C" {
#include "lz4dec.h"
}

// SystemWide Headers
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <libgen.h>

inline static bool isDir( QString path ) {
    struct stat statbuf;

    if ( stat( path.toLocal8Bit().data(), &statbuf ) == 0 ) {
        if ( S_ISDIR( statbuf.st_mode ) ) {
            return true;
        }

        else {
            return false;
        }
    }

    else {
        return false;
    }
}


inline static QStringList recDirWalk( QString path ) {
    QStringList fileList;

    if ( not isDir( path ) ) {
        return fileList;
    }

    QDirIterator it( path, QDir::AllEntries | QDir::System | QDir::NoDotAndDotDot | QDir::Hidden, QDirIterator::Subdirectories );

    while ( it.hasNext() ) {
        it.next();

        if ( it.fileInfo().isFile() ) {
            fileList.append( it.fileInfo().filePath() );
        }
    }

    return fileList;
}


inline static QString dirName( QString path ) {
    while ( path.contains( "//" ) ) {
        path = path.replace( "//", "/" );
    }

    if ( path.endsWith( "/" ) ) {
        path.chop( 1 );
    }

    char    *dupPath = strdup( path.toLocal8Bit().constData() );
    QString dirPth   = QString( dirname( dupPath ) ) + "/";

    free( dupPath );

    return (dirPth == "//" ? "/" : dirPth);
}


inline static QString baseName( QString path ) {
    while ( path.contains( "//" ) ) {
        path = path.replace( "//", "/" );
    }

    if ( path.endsWith( "/" ) ) {
        path.chop( 1 );
    }

    char    *dupPath = strdup( path.toLocal8Bit().constData() );
    QString basePth  = QString( basename( dupPath ) );

    free( dupPath );

    return basePth;
}


inline static bool exists( QString path ) {
    return not access( path.toLocal8Bit().constData(), F_OK );
}


inline static int mkpath( QString path, mode_t mode ) {
    /* Root always exists */
    if ( path == "/" ) {
        return 0;
    }

    /* If the directory exists, thats okay for us */
    if ( exists( path ) ) {
        return 0;
    }

    /* If the path is absolute, remove the leading '/' */
    if ( path.startsWith( '/' ) ) {
        path.remove( 0, 1 );
    }

    mkpath( dirName( path ), mode );

    return mkdir( path.toLocal8Bit().constData(), mode );
}


inline static QString longestPath( QStringList& dirs ) {
    QStringList paths;

    Q_FOREACH (QString file, dirs) {
        paths << QFileInfo( file ).absoluteFilePath();
    }

    /* Get shortest path: Shortest path is the one with least number of '/' */
    QString shortest = paths.at( 0 );
    int     count    = 10240;

    Q_FOREACH (QString path, paths) {
        if ( path.count( "/" ) < count ) {
            count    = path.count( "/" );
            shortest = path;
        }
    }

    /* Remove the trailing '/' */
    if ( shortest.endsWith( "/" ) ) {
        shortest.chop( 1 );
    }

    QFileInfo sDir( shortest );

    while ( paths.filter( sDir.absoluteFilePath() ).length() != paths.length() ) {
        if ( sDir.absoluteFilePath() == "/" ) {
            break;
        }

        sDir = QFileInfo( sDir.absolutePath() );
    }

    return sDir.absoluteFilePath();
}


LibArchiveQt::LibArchiveQt( QString archive ) {
    readDone        = false;
    isRunning       = false;
    mJob            = NoJob;
    extractedMember = QString();
    mExitStatus     = 0;            // 0 - Good, 1 - Bad

    archiveName = QDir( archive ).absolutePath();

    setFilterFormat( mimeDb.mimeTypeForFile( archiveName ) );
}


QString LibArchiveQt::suffix( QString archiveName ) {
    QMimeType mType = mimeDb.mimeTypeForFile( archiveName );

    if ( mType == mimeDb.mimeTypeForFile( "file.cpio" ) ) {
        return ".cpio";
    }

    else if ( mType == mimeDb.mimeTypeForFile( "file.shar" ) ) {
        return ".shar";
    }

    else if ( mType == mimeDb.mimeTypeForFile( "file.tar" ) ) {
        return ".tar";
    }

    else if ( mType == mimeDb.mimeTypeForFile( "file.tar.gz" ) ) {
        return (archiveName.endsWith( ".tar.gz" ) ? ".tar.gz" : ".tgz");
    }

    else if ( mType == mimeDb.mimeTypeForFile( "file.tar.grz" ) ) {
        return (archiveName.endsWith( ".tar.grz" ) ? ".tar.grz" : ".tgrz");
    }

    else if ( mType == mimeDb.mimeTypeForFile( "file.tar.xz" ) ) {
        return (archiveName.endsWith( ".tar.xz" ) ? ".tar.xz" : ".txz");
    }

    else if ( mType == mimeDb.mimeTypeForFile( "file.tar.lzo" ) ) {
        return (archiveName.endsWith( ".tar.lzo" ) ? ".tar.lzo" : ".tlzo");
    }

    else if ( mType == mimeDb.mimeTypeForFile( "file.tar.lzma" ) ) {
        return (archiveName.endsWith( ".tar.lzma" ) ? ".tar.lzma" : ".tlzma");
    }

    else if ( mType == mimeDb.mimeTypeForFile( "file.tar.lz" ) ) {
        return (archiveName.endsWith( ".tar.lz" ) ? ".tar.lz" : ".tlz");
    }

    else if ( mType == mimeDb.mimeTypeForFile( "file.tar.lrz" ) ) {
        return (archiveName.endsWith( ".tar.lrz" ) ? ".tar.lrz" : ".tlrz");
    }

    else if ( mType == mimeDb.mimeTypeForFile( "file.tar.lz4" ) ) {
        return (archiveName.endsWith( ".tar.lz4" ) ? ".tar.lz4" : ".tlzo4");
    }

    else if ( mType == mimeDb.mimeTypeForFile( "file.tar.bz2" ) ) {
        return (archiveName.endsWith( ".tar.bz2" ) ? ".tar.bz2" : ".tbz2");
    }

    else if ( mType == mimeDb.mimeTypeForFile( "file.tar.Z" ) ) {
        return (archiveName.endsWith( ".tar.Z" ) ? ".tar.Z" : ".tZ");
    }

    else if ( mType == mimeDb.mimeTypeForFile( "file.iso" ) ) {
        return ".iso";
    }

    else if ( mType == mimeDb.mimeTypeForFile( "file.zip" ) ) {
        return ".zip";
    }

    else if ( mType == mimeDb.mimeTypeForFile( "file.ar" ) ) {
        return ".ar";
    }

    else if ( mType == mimeDb.mimeTypeForFile( "file.xar" ) ) {
        return ".xar";
    }

    else if ( mType == mimeDb.mimeTypeForFile( "file.7z" ) ) {
        return ".7z";
    }

    else if ( mType == mimeDb.mimeTypeForFile( "file.lz" ) ) {
        return ".lz";
    }

    else if ( mType == mimeDb.mimeTypeForFile( "file.lz4" ) ) {
        return ".lz4";
    }

    else if ( mType == mimeDb.mimeTypeForFile( "file.uu" ) ) {
        return ".uu";
    }

    else if ( mType == mimeDb.mimeTypeForFile( "file.lzo" ) ) {
        return ".lzo";
    }

    else if ( mType == mimeDb.mimeTypeForFile( "file.gz" ) ) {
        return ".gz";
    }

    else if ( mType == mimeDb.mimeTypeForFile( "file.bz2" ) ) {
        return ".bz2";
    }

    else if ( mType == mimeDb.mimeTypeForFile( "file.lzma" ) ) {
        return ".lzma";
    }

    else if ( mType == mimeDb.mimeTypeForFile( "file.xz" ) ) {
        return ".xz";
    }

    return "";
}


QStringList LibArchiveQt::supportedFormats() {
    QStringList supported;

    supported << mimeDb.mimeTypeForFile( "file.cpio" ).name();
    supported << mimeDb.mimeTypeForFile( "file.shar" ).name();
    supported << mimeDb.mimeTypeForFile( "file.tar" ).name();
    supported << mimeDb.mimeTypeForFile( "file.tar.gz" ).name();
    supported << mimeDb.mimeTypeForFile( "file.tar.grz" ).name();
    supported << mimeDb.mimeTypeForFile( "file.tar.xz" ).name();
    supported << mimeDb.mimeTypeForFile( "file.tar.lzma" ).name();
    supported << mimeDb.mimeTypeForFile( "file.tar.lz4" ).name();
    supported << mimeDb.mimeTypeForFile( "file.tar.bz2" ).name();
    supported << mimeDb.mimeTypeForFile( "file.tar.Z" ).name();
    supported << mimeDb.mimeTypeForFile( "file.iso" ).name();
    supported << mimeDb.mimeTypeForFile( "file.zip" ).name();
    supported << mimeDb.mimeTypeForFile( "file.ar" ).name();
    supported << mimeDb.mimeTypeForFile( "file.xar" ).name();
    supported << mimeDb.mimeTypeForFile( "file.7z" ).name();
    supported << mimeDb.mimeTypeForFile( "file.lz" ).name();
    supported << mimeDb.mimeTypeForFile( "file.lz4" ).name();
    supported << mimeDb.mimeTypeForFile( "file.uu" ).name();
    supported << mimeDb.mimeTypeForFile( "file.lzo" ).name();
    supported << mimeDb.mimeTypeForFile( "file.gz" ).name();
    supported << mimeDb.mimeTypeForFile( "file.bz2" ).name();
    supported << mimeDb.mimeTypeForFile( "file.lzma" ).name();
    supported << mimeDb.mimeTypeForFile( "file.xz" ).name();

    QString binary;

#if QT_VERSION >= QT_VERSION_CHECK( 5, 14, 0 )
    QStringList exeLocs = QString::fromLocal8Bit( qgetenv( "PATH" ) ).split( ":", Qt::SkipEmptyParts );
#else
    QStringList exeLocs = QString::fromLocal8Bit( qgetenv( "PATH" ) ).split( ":", QString::SkipEmptyParts );
#endif

    QString lzop  = mimeDb.mimeTypeForFile( "file.lzo" ).name();
    QString lzip  = mimeDb.mimeTypeForFile( "file.lz" ).name();
    QString lrzip = mimeDb.mimeTypeForFile( "file.lrz" ).name();

    Q_FOREACH (QString loc, exeLocs) {
        if ( exists( loc + "/lzip" ) and not supported.contains( lzip ) ) {
            supported << lzip;
        }

        if ( exists( loc + "/lzop" ) and not supported.contains( lzop ) ) {
            supported << lzop;
        }

        if ( exists( loc + "/lrzip" ) and not supported.contains( lrzip ) ) {
            supported << lrzip;
        }
    }

    return supported;
}


void LibArchiveQt::createArchive() {
    mJob      = CreateArchive;
    isRunning = true;

    start();
}


void LibArchiveQt::extractArchive() {
    mJob      = ExtractArchive;
    isRunning = true;

    start();
}


void LibArchiveQt::extractMember( QString memberName ) {
    extractedMember = memberName;
    mJob            = ExtractMember;
    isRunning       = true;

    start();
}


ArchiveEntries LibArchiveQt::listArchive() {
    if ( readDone ) {
        return memberList;
    }

    memberList.clear();

    struct archive       *a;
    struct archive_entry *entry;
    int r;

    // Source Archive
    a = archive_read_new();
    archive_read_support_format_all( a );
    archive_read_support_format_raw( a );
    archive_read_support_filter_all( a );

    if ( (r = archive_read_open_filename( a, archiveName.toUtf8().data(), 10240 ) ) ) {
        qDebug() << "[Error]" << archive_error_string( a );
        readDone = true;
        return ArchiveEntries();
    }

    while ( true ) {
        r = archive_read_next_header( a, &entry );

        if ( r == ARCHIVE_EOF ) {
            break;
        }

        if ( r < ARCHIVE_OK ) {
            qDebug() << archive_error_string( a );
        }

        ArchiveEntry *ae = new ArchiveEntry;
        ae->name = archive_entry_pathname( entry );
        ae->size = archive_entry_size( entry );
        ae->type = archive_entry_filetype( entry );
        memcpy( &ae->info, archive_entry_stat( entry ), sizeof(struct stat) );

        memberList << ae;
    }

    archive_read_close( a );
    archive_read_free( a );

    readDone = true;

    return memberList;
}


int LibArchiveQt::exitStatus() {
    return mExitStatus;
}


void LibArchiveQt::updateInputFiles( QStringList inFiles, LibArchiveQt::InputFileMode inMode ) {
    if ( not inFiles.length() ) {
        return;
    }

    /* First get the absolute filenames */
    Q_FOREACH (QString file, inFiles) {
        if ( isDir( file ) ) {
            updateInputFiles( recDirWalk( file ), inMode );
        }

        else {
            QFileInfo info( file );
            switch ( inMode ) {
                case AbsolutePath: {
                    inputList.insert( info.absoluteFilePath(), info.absoluteFilePath() );
                    break;
                }

                case RelativeToRoot: {
                    inputList.insert( info.absoluteFilePath(), QDir::root().relativeFilePath( info.absoluteFilePath() ) );
                    break;
                }

                case RelativeToHome: {
                    QString relPath = QDir::home().relativeFilePath( info.absoluteFilePath() );
                    while ( relPath.startsWith( "../" ) ) {
                        relPath.remove( 0, 3 );
                    }

                    inputList.insert( info.absoluteFilePath(), relPath );
                    break;
                }

                case RelativeToCurrent: {
                    QString relPath = QDir::current().relativeFilePath( info.absoluteFilePath() );
                    while ( relPath.startsWith( "../" ) ) {
                        relPath.remove( 0, 3 );
                    }

                    inputList.insert( info.absoluteFilePath(), relPath );
                    break;
                }

                case RelativeToWorkDir: {
                    /* If @src is empty, set it to root */
                    src = (src.isEmpty() ? "/" : src);

                    QString relPath = QDir( src ).relativeFilePath( info.absoluteFilePath() );
                    while ( relPath.startsWith( "../" ) ) {
                        relPath.remove( 0, 3 );
                    }

                    inputList.insert( info.absoluteFilePath(), relPath );
                    break;
                }

                case CommonRelativePath: {
                    QString common;

                    if ( inFiles.length() == 1 ) {
                        common = dirName( inFiles.at( 0 ) );
                    }

                    else {
                        common = longestPath( inFiles );
                    }

                    QString relPath = QDir( common ).relativeFilePath( info.absoluteFilePath() );
                    while ( relPath.startsWith( "../" ) ) {
                        relPath.remove( 0, 3 );
                    }

                    inputList.insert( info.absoluteFilePath(), relPath );
                    break;
                }

                default: {
                    QString relPath = QDir::current().relativeFilePath( info.absoluteFilePath() );
                    while ( relPath.startsWith( "../" ) ) {
                        relPath.remove( 0, 3 );
                    }

                    inputList.insert( info.absoluteFilePath(), relPath );
                    break;
                }
            }
        }
    }
}


void LibArchiveQt::setWorkingDir( QString wDir ) {
    src = QString( wDir );
}


void LibArchiveQt::setDestination( QString path ) {
    /*
     *
     * @p path will be a absolute.
     * So QDir we construct will be home path
     *
     */

    dest = QString( path );

    if ( not QFileInfo( QDir( dest ).absolutePath() ).exists() ) {
        mkpath( path, 0755 );
    }

    qDebug() << "Extracting to:" << dest;
}


void LibArchiveQt::waitForFinished() {
    if ( not isRunning ) {
        return;
    }

    QEventLoop eventLoop;

#if QT_VERSION >= 0x050000
    connect( this, &LibArchiveQt::jobFailed,   &eventLoop, &QEventLoop::quit );
    connect( this, &LibArchiveQt::jobComplete, &eventLoop, &QEventLoop::quit );
#else
    connect( this, SIGNAL(jobFailed()),        &eventLoop, SLOT( quit() ) );
    connect( this, SIGNAL(jobComplete()),      &eventLoop, SLOT( quit() ) );
#endif

    eventLoop.exec();
}


void LibArchiveQt::run() {
    switch ( mJob ) {
        case CreateArchive: {
            if ( doCreateArchive() ) {
                mExitStatus = 0;
                emit jobComplete();
            }

            else {
                mExitStatus = 1;
                emit jobFailed();
            }

            isRunning = false;
            return;
        }

        case ExtractArchive: {
            if ( doExtractArchive() ) {
                mExitStatus = 0;
                emit jobComplete();
            }

            else {
                mExitStatus = 1;
                emit jobFailed();
            }

            isRunning = false;
            return;
        }

        case ExtractMember: {
            if ( doExtractMember( extractedMember ) ) {
                mExitStatus = 0;
                emit jobComplete();
            }

            else {
                mExitStatus = 1;
                emit jobFailed();
            }

            isRunning = false;
            return;
        }

        case ListArchive: {
            /* Nothing to run in the thread */
            return;
        }
    }
}


bool LibArchiveQt::doCreateArchive() {
    struct archive       *a;
    struct archive_entry *entry;
    struct stat          st;
    char                 buff[ 8192 ];
    int len;
    int fd;
    int r         = ARCHIVE_OK;
    int errors    = 0;
    int processed = 0;

    /* Prepare the workingDir */
    if ( src.isEmpty() ) {
        src = "/";
    }

    a = archive_write_new();

    // Depend on the format provided by the user
    r |= archive_write_set_format( a, mArchiveFormat );
    r |= archive_write_add_filter( a, mArchiveFilter );

    if ( r < ARCHIVE_OK ) {
        qDebug() << "Cannot use the input filter/format.";
        return false;
    }

    r = archive_write_open_filename( a, archiveName.toUtf8().data() );

    if ( r < ARCHIVE_OK ) {
        qDebug() << "Unable to write file for writing.";
        return false;
    }

    Q_FOREACH (QString file, inputList.keys() ) {
        char *filename = new char[ file.length() + 1 ];
        strcpy( filename, file.toUtf8().data() );

        if ( stat( filename, &st ) != 0 ) {
            errors++;
            printf( "[Error %d]: %s: %s\n", errno, strerror( errno ), filename );
            continue;
        }

        char *arcPath = new char[ file.length() + 1 ];
        strcpy( arcPath, inputList.value( file ).toUtf8().data() );

        entry = archive_entry_new();
        archive_entry_set_pathname( entry, arcPath );
        archive_entry_set_size( entry, st.st_size );
        archive_entry_set_filetype( entry, st.st_mode );
        archive_entry_set_perm( entry, st.st_mode );

        archive_write_header( a, entry );

        // Perform the write
        fd  = open( filename, O_RDONLY );
        len = read( fd, buff, sizeof(buff) );
        while ( len > 0 ) {
            archive_write_data( a, buff, len );
            len = read( fd, buff, sizeof(buff) );
        }
        close( fd );
        archive_entry_free( entry );

        processed++;

        emit progress( processed * 100 / inputList.count() );
        qApp->processEvents();
    }

    archive_write_close( a );
    archive_write_free( a );

    return (errors ? false : true);
}


bool LibArchiveQt::doExtractArchive() {
    if ( archiveType == None ) {
        return false;
    }

    // Change to the target directory
    char srcDir[ 10240 ] = { 0 };

    getcwd( srcDir, 10240 );

    if ( not dest.isEmpty() ) {
        int ret = chdir( dest.toUtf8().data() );

        if ( ret ) {
            qDebug() << "chdir() failed:" << errno;
        }
    }

    if ( archiveType == Single ) {
        QMimeType mType = mimeDb.mimeTypeForFile( archiveName );

        if ( mType == mimeDb.mimeTypeForFile( "file.lz" ) ) {
            /* LZip Extractor */

            QString lzip;

#if QT_VERSION >= QT_VERSION_CHECK( 5, 14, 0 )
            QStringList exeLocs = QString::fromLocal8Bit( qgetenv( "PATH" ) ).split( ":", Qt::SkipEmptyParts );
#else
            QStringList exeLocs = QString::fromLocal8Bit( qgetenv( "PATH" ) ).split( ":", QString::SkipEmptyParts );
#endif

            Q_FOREACH (QString loc, exeLocs) {
                if ( exists( loc + "/lzip" ) ) {
                    lzip = loc + "/lzip";
                    break;
                }
            }

            if ( not lzip.length() ) {
                qDebug() << "External program lzip not found.";
                return false;
            }

            struct archive       *a;
            struct archive       *ext;
            struct archive_entry *entry;
            int flags;

            int r = ARCHIVE_OK;

            /* Select which attributes we want to restore. */
            flags  = ARCHIVE_EXTRACT_TIME;
            flags |= ARCHIVE_EXTRACT_PERM;
            flags |= ARCHIVE_EXTRACT_ACL;
            flags |= ARCHIVE_EXTRACT_FFLAGS;

            // Source Archive
            a  = archive_read_new();
            r |= archive_read_support_format_raw( a );
            r |= archive_read_support_filter_program( a, QString( lzip + " -d" ).toLocal8Bit().data() );

            if ( r < ARCHIVE_OK ) {
                qDebug() << "Cannot use the input filter/format.";
                return false;
            }

            // Structure to write files to disk
            ext = archive_write_disk_new();
            archive_write_disk_set_options( ext, flags );
            archive_write_disk_set_standard_lookup( ext );

            if ( (r = archive_read_open_filename( a, archiveName.toLocal8Bit().data(), 10240 ) ) ) {
                qDebug() << "Unable to read archive:" << archive_error_string( a );
                return false;
            }

            while ( true ) {
                r = archive_read_next_header( a, &entry );

                if ( r == ARCHIVE_EOF ) {
                    qDebug() << "EOF";
                    break;
                }

                if ( r < ARCHIVE_OK ) {
                    fprintf( stderr, "%s\n", archive_error_string( a ) );
                }

                if ( r < ARCHIVE_WARN ) {
                    fprintf( stderr, "%s\n", archive_error_string( a ) );
                    return false;
                }

                r = archive_write_header( ext, entry );

                if ( r < ARCHIVE_OK ) {
                    fprintf( stderr, "%s\n", archive_error_string( ext ) );
                }

                else if ( archive_entry_size( entry ) == 0 ) {
                    r = copyData( a, ext );

                    if ( r < ARCHIVE_OK ) {
                        fprintf( stderr, "%s\n", archive_error_string( ext ) );
                    }

                    if ( r < ARCHIVE_WARN ) {
                        return false;
                    }
                }

                r = archive_write_finish_entry( ext );

                if ( r < ARCHIVE_OK ) {
                    fprintf( stderr, "%s\n", archive_error_string( ext ) );
                }

                if ( r < ARCHIVE_WARN ) {
                    return true;
                }
            }

            archive_read_close( a );
            archive_read_free( a );

            archive_write_close( ext );
            archive_write_free( ext );

            return true;
        }

        else if ( mType == mimeDb.mimeTypeForFile( "file.uu" ) ) {
            /* UUEncode Extractor */

            return false;
        }

        else if ( mType == mimeDb.mimeTypeForFile( "file.lrz" ) ) {
            /* lrzip Extractor */

            QString lrzip;

#if QT_VERSION >= QT_VERSION_CHECK( 5, 14, 0 )
            QStringList exeLocs = QString::fromLocal8Bit( qgetenv( "PATH" ) ).split( ":", Qt::SkipEmptyParts );
#else
            QStringList exeLocs = QString::fromLocal8Bit( qgetenv( "PATH" ) ).split( ":", QString::SkipEmptyParts );
#endif
            Q_FOREACH (QString loc, exeLocs) {
                if ( exists( loc + "/lrzip" ) ) {
                    lrzip = loc + "/lrzip";
                    break;
                }
            }

            if ( not lrzip.length() ) {
                qDebug() << "External program lrzip not found.";
                return false;
            }

            struct archive       *a;
            struct archive       *ext;
            struct archive_entry *entry;
            int flags;

            int r = ARCHIVE_OK;

            /* Select which attributes we want to restore. */
            flags  = ARCHIVE_EXTRACT_TIME;
            flags |= ARCHIVE_EXTRACT_PERM;
            flags |= ARCHIVE_EXTRACT_ACL;
            flags |= ARCHIVE_EXTRACT_FFLAGS;

            // Source Archive
            a  = archive_read_new();
            r |= archive_read_support_format_raw( a );
            r |= archive_read_support_filter_program( a, QString( lrzip + " -d" ).toLocal8Bit().data() );

            if ( r < ARCHIVE_OK ) {
                qDebug() << "Cannot use the input filter/format.";
            }

            // Structure to write files to disk
            ext = archive_write_disk_new();
            archive_write_disk_set_options( ext, flags );
            archive_write_disk_set_standard_lookup( ext );

            if ( (r = archive_read_open_filename( a, archiveName.toLocal8Bit().data(), 10240 ) ) ) {
                qDebug() << "Unable to read archive:" << archive_error_string( a );
                return false;
            }

            while ( true ) {
                r = archive_read_next_header( a, &entry );

                if ( r == ARCHIVE_EOF ) {
                    qDebug() << "EOF";
                    break;
                }

                if ( r < ARCHIVE_OK ) {
                    fprintf( stderr, "%s\n", archive_error_string( a ) );
                }

                if ( r < ARCHIVE_WARN ) {
                    fprintf( stderr, "%s\n", archive_error_string( a ) );
                    return false;
                }

                r = archive_write_header( ext, entry );

                if ( r < ARCHIVE_OK ) {
                    fprintf( stderr, "%s\n", archive_error_string( ext ) );
                }

                else if ( archive_entry_size( entry ) == 0 ) {
                    r = copyData( a, ext );

                    if ( r < ARCHIVE_OK ) {
                        fprintf( stderr, "%s\n", archive_error_string( ext ) );
                    }

                    if ( r < ARCHIVE_WARN ) {
                        return false;
                    }
                }

                r = archive_write_finish_entry( ext );

                if ( r < ARCHIVE_OK ) {
                    fprintf( stderr, "%s\n", archive_error_string( ext ) );
                }

                if ( r < ARCHIVE_WARN ) {
                    return true;
                }
            }

            archive_read_close( a );
            archive_read_free( a );

            archive_write_close( ext );
            archive_write_free( ext );

            return true;
        }

        else if ( mType == mimeDb.mimeTypeForFile( "file.lzo" ) ) {
            /* LZop Extractor */

            QString lzop;

#if QT_VERSION >= QT_VERSION_CHECK( 5, 14, 0 )
            QStringList exeLocs = QString::fromLocal8Bit( qgetenv( "PATH" ) ).split( ":", Qt::SkipEmptyParts );
#else
            QStringList exeLocs = QString::fromLocal8Bit( qgetenv( "PATH" ) ).split( ":", QString::SkipEmptyParts );
#endif
            Q_FOREACH (QString loc, exeLocs) {
                if ( exists( loc + "/lzop" ) ) {
                    lzop = loc + "/lzop";
                    break;
                }
            }

            if ( not lzop.length() ) {
                qDebug() << "External program lzop not found.";
                return false;
            }

            struct archive       *a;
            struct archive       *ext;
            struct archive_entry *entry;
            int flags;

            int r = ARCHIVE_OK;

            /* Select which attributes we want to restore. */
            flags  = ARCHIVE_EXTRACT_TIME;
            flags |= ARCHIVE_EXTRACT_PERM;
            flags |= ARCHIVE_EXTRACT_ACL;
            flags |= ARCHIVE_EXTRACT_FFLAGS;

            // Source Archive
            a  = archive_read_new();
            r |= archive_read_support_format_raw( a );
            r |= archive_read_support_filter_program( a, QString( lzop + " -d" ).toLocal8Bit().data() );

            if ( r < ARCHIVE_OK ) {
                qDebug() << "Cannot use the input filter/format.";
            }

            // Structure to write files to disk
            ext = archive_write_disk_new();
            archive_write_disk_set_options( ext, flags );
            archive_write_disk_set_standard_lookup( ext );

            if ( (r = archive_read_open_filename( a, archiveName.toLocal8Bit().data(), 10240 ) ) ) {
                qDebug() << "Unable to read archive:" << archive_error_string( a );
                return false;
            }

            while ( true ) {
                r = archive_read_next_header( a, &entry );

                if ( r == ARCHIVE_EOF ) {
                    qDebug() << "EOF";
                    break;
                }

                if ( r < ARCHIVE_OK ) {
                    fprintf( stderr, "%s\n", archive_error_string( a ) );
                }

                if ( r < ARCHIVE_WARN ) {
                    fprintf( stderr, "%s\n", archive_error_string( a ) );
                    return false;
                }

                r = archive_write_header( ext, entry );

                if ( r < ARCHIVE_OK ) {
                    fprintf( stderr, "%s\n", archive_error_string( ext ) );
                }

                else if ( archive_entry_size( entry ) == 0 ) {
                    r = copyData( a, ext );

                    if ( r < ARCHIVE_OK ) {
                        fprintf( stderr, "%s\n", archive_error_string( ext ) );
                    }

                    if ( r < ARCHIVE_WARN ) {
                        return false;
                    }
                }

                r = archive_write_finish_entry( ext );

                if ( r < ARCHIVE_OK ) {
                    fprintf( stderr, "%s\n", archive_error_string( ext ) );
                }

                if ( r < ARCHIVE_WARN ) {
                    return true;
                }
            }

            archive_read_close( a );
            archive_read_free( a );

            archive_write_close( ext );
            archive_write_free( ext );

            return true;
        }

        else if ( mType == mimeDb.mimeTypeForFile( "file.lz4" ) ) {
            /* LZ4 Extractor */

            dest = archiveName;
            dest.chop( 4 );

            int i = 0;
            while ( exists( dest ) ) {
                i++;

                dest = archiveName;
                dest.chop( 3 );

                dest = dirName( dest ) + QString( "(%1) - " ).arg( i ) + baseName( dest );
            }

            unlz4( archiveName.toLocal8Bit().constData(), dest.toLocal8Bit().constData(), NULL );
            return true;
        }

        else if ( mType == mimeDb.mimeTypeForFile( "file.gz" ) ) {
            /* GZip Extractor */

            dest = archiveName;
            dest.chop( 3 );

            int i = 0;
            while ( exists( dest ) ) {
                i++;

                dest = archiveName;
                dest.chop( 3 );

                dest = dirName( dest ) + QString( "(%1) - " ).arg( i ) + baseName( dest );
            }

            NBGZip *gzExt = new NBGZip( archiveName, dest );
            return gzExt->extract();
        }

        else if ( mType == mimeDb.mimeTypeForFile( "file.bz2" ) ) {
            /* BZip2 Extractor */

            dest = archiveName;
            dest.chop( 3 );

            int i = 0;
            while ( exists( dest ) ) {
                i++;

                dest = archiveName;
                dest.chop( 3 );

                dest = dirName( dest ) + QString( "(%1) - " ).arg( i ) + baseName( dest );
            }

            NBBZip2 *bz2Ext = new NBBZip2( archiveName, dest );
            return bz2Ext->extract();
        }

        else if ( mType == mimeDb.mimeTypeForFile( "file.lzma" ) ) {
            /* LZMA Extractor */

            dest = archiveName;
            dest.chop( 3 );

            int i = 0;
            while ( exists( dest ) ) {
                i++;

                dest = archiveName;
                dest.chop( 3 );

                dest = dirName( dest ) + QString( "(%1) - " ).arg( i ) + baseName( dest );
            }

            NBLzma *lzmaExt = new NBLzma( archiveName, dest );
            return lzmaExt->extract();
        }

        else if ( mType == mimeDb.mimeTypeForFile( "file.xz" ) ) {
            /* XZ Extractor */

            dest = archiveName;
            dest.chop( 3 );

            int i = 0;
            while ( exists( dest ) ) {
                i++;

                dest = archiveName;
                dest.chop( 3 );

                dest = dirName( dest ) + QString( "(%1) - " ).arg( i ) + baseName( dest );
            }

            NBXz *xzExt = new NBXz( archiveName, dest );
            return xzExt->extract();
        }

        return false;
    }

    else {
        /*
         * To show progress we want the number of entries. So list the archive first.
         * Then count the number of members. Then clear the memberList
         */
        listArchive();
        int entryCount = memberList.length();
        memberList.clear();
        readDone = false;

        struct archive       *a;
        struct archive       *ext;
        struct archive_entry *entry;
        int flags;
        int r = ARCHIVE_OK;

        int processedEntries = 0;

        /* Select which attributes we want to restore. */
        flags  = ARCHIVE_EXTRACT_TIME;
        flags |= ARCHIVE_EXTRACT_PERM;
        flags |= ARCHIVE_EXTRACT_ACL;
        flags |= ARCHIVE_EXTRACT_FFLAGS;

        // Source Archive
        a = archive_read_new();

        r |= archive_read_support_format_all( a );
        r |= archive_read_support_filter_all( a );

        if ( (r |= archive_read_open_filename( a, archiveName.toUtf8().data(), 10240 ) ) < ARCHIVE_OK ) {
            fprintf( stderr, "%s\n", archive_error_string( a ) );
            return false;
        }

        r = ARCHIVE_OK;

        // Structure to write files to disk
        ext = archive_write_disk_new();
        r  |= archive_write_disk_set_options( ext, flags );
        r  |= archive_write_disk_set_standard_lookup( ext );

        if ( r < ARCHIVE_WARN ) {
            fprintf( stderr, "%s\n", archive_error_string( a ) );
            return false;
        }

        while ( true ) {
            r = archive_read_next_header( a, &entry );

            if ( r == ARCHIVE_EOF ) {
                break;
            }

            if ( r < ARCHIVE_OK ) {
                fprintf( stderr, "%s\n", archive_error_string( a ) );
            }

            if ( r < ARCHIVE_WARN ) {
                return 1;
            }

            r = archive_write_header( ext, entry );

            if ( r < ARCHIVE_OK ) {
                fprintf( stderr, "%s\n", archive_error_string( ext ) );
            }

            else if ( archive_entry_size( entry ) > 0 ) {
                r = copyData( a, ext );

                if ( r < ARCHIVE_OK ) {
                    fprintf( stderr, "%s\n", archive_error_string( ext ) );
                }

                if ( r < ARCHIVE_WARN ) {
                    return 1;
                }
            }

            processedEntries++;
            emit progress( processedEntries * 100 / entryCount );

            qApp->processEvents();

            r = archive_write_finish_entry( ext );

            if ( r < ARCHIVE_OK ) {
                fprintf( stderr, "%s\n", archive_error_string( ext ) );
            }

            if ( r < ARCHIVE_WARN ) {
                return 1;
            }
        }

        archive_read_close( a );
        archive_read_free( a );

        archive_write_close( ext );
        archive_write_free( ext );

        return true;
    }

    chdir( srcDir );
}


bool LibArchiveQt::doExtractMember( QString memberName ) {
    listArchive();

    if ( archiveType == Single ) {
        return doExtractMember( memberName );
    }

    // Change to the target directory
    char srcDir[ 10240 ] = { 0 };

    getcwd( srcDir, 10240 );
    chdir( dest.toUtf8().data() );

    struct archive       *a;
    struct archive       *ext;
    struct archive_entry *entry;
    int flags;
    int r;

    /* Select which attributes we want to restore. */
    flags  = ARCHIVE_EXTRACT_TIME;
    flags |= ARCHIVE_EXTRACT_PERM;
    flags |= ARCHIVE_EXTRACT_ACL;
    flags |= ARCHIVE_EXTRACT_FFLAGS;

    // Source Archive
    a = archive_read_new();
    archive_read_support_format_all( a );
    archive_read_support_filter_all( a );

    // Structure to write files to disk
    ext = archive_write_disk_new();
    archive_write_disk_set_options( ext, flags );
    archive_write_disk_set_standard_lookup( ext );

    r = archive_read_open_filename( a, archiveName.toUtf8().data(), 10240 );

    if ( r != ARCHIVE_OK ) {
        qDebug() << "[ERROR]: Failed to open archive:" << archiveName;
        return true;
    }

    bool dir = false, found = false;

    /* Direct member */
    Q_FOREACH (ArchiveEntry *ae, memberList) {
        if ( ae->name == memberName ) {
            dir   = (ae->type == AE_IFDIR);
            found = true;
            break;
        }

        if ( ae->name == memberName + "/" ) {
            memberName += "/";
            dir         = (ae->type == AE_IFDIR);
            found       = true;
            break;
        }
    }

    if ( not found ) {
        /* Always check for @memberName + "/" because, all indirect members will be directories */
        memberName += "/";

        /* Indirect member: ex. debug/ is a member if debug/path/to/file.ext exists */
        Q_FOREACH (ArchiveEntry *ae, memberList) {
            if ( ae->name.startsWith( memberName ) == 0 ) {
                dir   = true;
                found = true;
                break;
            }
        }
    }

    if ( found ) {
        while ( true ) {
            r = archive_read_next_header( a, &entry );

            if ( r == ARCHIVE_EOF ) {
                break;
            }

            if ( r < ARCHIVE_OK ) {
                fprintf( stderr, "%s\n", archive_error_string( a ) );
            }

            if ( r < ARCHIVE_WARN ) {
                return true;
            }

            QString entryPath = archive_entry_pathname( entry );

            /* Check if the current entry starts with @memberName */
            if ( entryPath.startsWith( memberName ) ) {
                if ( not dir ) {
                    if ( entryPath != memberName ) {
                        continue;
                    }
                }

                r = archive_write_header( ext, entry );

                if ( r < ARCHIVE_OK ) {
                    fprintf( stderr, "%s\n", archive_error_string( ext ) );
                }

                else if ( archive_entry_size( entry ) > 0 ) {
                    r = copyData( a, ext );

                    if ( r < ARCHIVE_OK ) {
                        fprintf( stderr, "%s\n", archive_error_string( ext ) );
                    }

                    if ( r < ARCHIVE_WARN ) {
                        return false;
                    }
                }

                r = archive_write_finish_entry( ext );

                if ( r < ARCHIVE_OK ) {
                    fprintf( stderr, "%s\n", archive_error_string( ext ) );
                }

                if ( r < ARCHIVE_WARN ) {
                    return false;
                }
            }
        }
    }

    else {
        qDebug() << "[Error]" << "File not found in the archive:" << memberName;
        return false;
    }

    archive_read_close( a );
    archive_read_free( a );

    archive_write_close( ext );
    archive_write_free( ext );

    chdir( srcDir );

    return true;
}


int LibArchiveQt::copyData( struct archive *ar, struct archive *aw ) {
    int        r;
    const void *buff;
    size_t     size;

#ifdef __LP64__
    off_t offset;
#else
    la_int64_t offset;
#endif

    while ( true ) {
        r = archive_read_data_block( ar, &buff, &size, &offset );

        if ( r == ARCHIVE_EOF ) {
            return (ARCHIVE_OK);
        }

        if ( r < ARCHIVE_OK ) {
            return (r);
        }

        r = archive_write_data_block( aw, buff, size, offset );

        if ( r < ARCHIVE_OK ) {
            fprintf( stderr, "%s\n", archive_error_string( aw ) );
            return (r);
        }
    }
}


void LibArchiveQt::setFilterFormat( QMimeType mType ) {
    if ( mType == mimeDb.mimeTypeForFile( "file.cpio" ) ) {
        mArchiveFilter = ARCHIVE_FILTER_NONE;
        mArchiveFormat = ARCHIVE_FORMAT_CPIO;
        archiveType    = Container;
    }

    else if ( mType == mimeDb.mimeTypeForFile( "file.shar" ) ) {
        mArchiveFilter = ARCHIVE_FILTER_NONE;
        mArchiveFormat = ARCHIVE_FORMAT_SHAR;
        archiveType    = Container;
    }

    else if ( mType == mimeDb.mimeTypeForFile( "file.tar" ) ) {
        mArchiveFilter = ARCHIVE_FILTER_NONE;
        mArchiveFormat = ARCHIVE_FORMAT_TAR;
        archiveType    = Container;
    }

    else if ( mType == mimeDb.mimeTypeForFile( "file.tar.gz" ) ) {
        mArchiveFilter = ARCHIVE_FILTER_GZIP;
        mArchiveFormat = ARCHIVE_FORMAT_TAR;
        archiveType    = Container;
    }

    else if ( mType == mimeDb.mimeTypeForFile( "file.tar.grz" ) ) {
        mArchiveFilter = ARCHIVE_FILTER_GRZIP;
        mArchiveFormat = ARCHIVE_FORMAT_TAR;
        archiveType    = Container;
    }

    else if ( mType == mimeDb.mimeTypeForFile( "file.tar.xz" ) ) {
        mArchiveFilter = ARCHIVE_FILTER_XZ;
        mArchiveFormat = ARCHIVE_FORMAT_TAR;
        archiveType    = Container;
    }

    else if ( mType == mimeDb.mimeTypeForFile( "file.tar.lzo" ) ) {
        mArchiveFilter = ARCHIVE_FILTER_LZOP;
        mArchiveFormat = ARCHIVE_FORMAT_TAR;
        archiveType    = Container;
    }

    else if ( mType == mimeDb.mimeTypeForFile( "file.tar.lzma" ) ) {
        mArchiveFilter = ARCHIVE_FILTER_LZMA;
        mArchiveFormat = ARCHIVE_FORMAT_TAR;
        archiveType    = Container;
    }

    else if ( mType == mimeDb.mimeTypeForFile( "file.tar.lz" ) ) {
        mArchiveFilter = ARCHIVE_FILTER_LZIP;
        mArchiveFormat = ARCHIVE_FORMAT_TAR;
        archiveType    = Container;
    }

    else if ( mType == mimeDb.mimeTypeForFile( "file.tar.lrz" ) ) {
        mArchiveFilter = ARCHIVE_FILTER_LRZIP;
        mArchiveFormat = ARCHIVE_FORMAT_TAR;
        archiveType    = Container;
    }

#if ARCHIVE_VERSION_NUMBER > 3001002
    else if ( mType == mimeDb.mimeTypeForFile( "file.tar.lz4" ) ) {
        mArchiveFilter = ARCHIVE_FILTER_LZ4;
        mArchiveFormat = ARCHIVE_FORMAT_TAR;
        archiveType    = Container;
    }
#endif

    else if ( mType == mimeDb.mimeTypeForFile( "file.tar.bz2" ) ) {
        mArchiveFilter = ARCHIVE_FILTER_BZIP2;
        mArchiveFormat = ARCHIVE_FORMAT_TAR;
        archiveType    = Container;
    }

    else if ( mType == mimeDb.mimeTypeForFile( "file.tar.Z" ) ) {
        mArchiveFilter = ARCHIVE_FILTER_COMPRESS;
        mArchiveFormat = ARCHIVE_FORMAT_TAR;
        archiveType    = Container;
    }

    else if ( mType == mimeDb.mimeTypeForFile( "file.iso" ) ) {
        mArchiveFilter = ARCHIVE_FILTER_NONE;
        mArchiveFormat = ARCHIVE_FORMAT_ISO9660;
        archiveType    = Container;
    }

    else if ( mType == mimeDb.mimeTypeForFile( "file.zip" ) ) {
        mArchiveFilter = ARCHIVE_FILTER_NONE;
        mArchiveFormat = ARCHIVE_FORMAT_ZIP;
        archiveType    = Container;
    }

    else if ( mType == mimeDb.mimeTypeForFile( "file.ar" ) ) {
        mArchiveFilter = ARCHIVE_FILTER_NONE;
        mArchiveFormat = ARCHIVE_FORMAT_AR;
        archiveType    = Container;
    }

    else if ( mType == mimeDb.mimeTypeForFile( "file.xar" ) ) {
        mArchiveFilter = ARCHIVE_FILTER_NONE;
        mArchiveFormat = ARCHIVE_FORMAT_XAR;
        archiveType    = Container;
    }

    else if ( mType == mimeDb.mimeTypeForFile( "file.7z" ) ) {
        mArchiveFilter = ARCHIVE_FILTER_NONE;
        mArchiveFormat = ARCHIVE_FORMAT_7ZIP;
        archiveType    = Container;
    }

    else if ( mType == mimeDb.mimeTypeForFile( "file.lz" ) ) {
        mArchiveFilter = ARCHIVE_FILTER_LZIP;
        mArchiveFormat = ARCHIVE_FORMAT_RAW;
        archiveType    = Single;
    }

#if ARCHIVE_VERSION_NUMBER > 3001002
    else if ( mType == mimeDb.mimeTypeForFile( "file.lz4" ) ) {
        mArchiveFilter = ARCHIVE_FILTER_LZ4;
        mArchiveFormat = ARCHIVE_FORMAT_RAW;
        archiveType    = Single;
    }
#endif

    else if ( mType == mimeDb.mimeTypeForFile( "file.uu" ) ) {
        mArchiveFilter = ARCHIVE_FILTER_UU;
        mArchiveFormat = ARCHIVE_FORMAT_RAW;
        archiveType    = Single;
    }

    else if ( mType == mimeDb.mimeTypeForFile( "file.lzo" ) ) {
        mArchiveFilter = ARCHIVE_FILTER_LZOP;
        mArchiveFormat = ARCHIVE_FORMAT_RAW;
        archiveType    = Single;
    }

    else if ( mType == mimeDb.mimeTypeForFile( "file.gz" ) ) {
        mArchiveFilter = ARCHIVE_FILTER_GZIP;
        mArchiveFormat = ARCHIVE_FORMAT_RAW;
        archiveType    = Single;
    }

    else if ( mType == mimeDb.mimeTypeForFile( "file.bz2" ) ) {
        mArchiveFilter = ARCHIVE_FILTER_BZIP2;
        mArchiveFormat = ARCHIVE_FORMAT_RAW;
        archiveType    = Single;
    }

    else if ( mType == mimeDb.mimeTypeForFile( "file.lzma" ) ) {
        mArchiveFilter = ARCHIVE_FILTER_LZMA;
        mArchiveFormat = ARCHIVE_FORMAT_RAW;
        archiveType    = Single;
    }

    else if ( mType == mimeDb.mimeTypeForFile( "file.xz" ) ) {
        mArchiveFilter = ARCHIVE_FILTER_XZ;
        mArchiveFormat = ARCHIVE_FORMAT_RAW;
        archiveType    = Single;
    }

    else {
        mArchiveFormat = ARCHIVE_FORMAT_EMPTY;
        mArchiveFilter = ARCHIVE_FILTER_NONE;
        archiveType    = None;
    }
}
