Headers = [
  	'LibLzma.hpp',
  	'LibLzma2.hpp',
  	'LibBZip2.hpp',
  	'LibGZip.hpp',
  	'lz4dec.h',
  	'libarchiveqt.h'
]

Sources = [
  	'LibArchive.cpp',
  	'LibLzma.cpp',
  	'LibLzma2.cpp',
  	'LibBZip2.cpp',
  	'LibGZip.cpp',
  	'lz4dec.c'
]

if ( QtCore.version().startswith( '4.' ) )
    warning( 'We strongly advice against using Qt4.' )

    Headers += [
        'MimeHandler/NBMimeDatabase.hpp',
        'MimeHandler/NBMimeDatabase_p.hpp',
        'MimeHandler/NBMimeGlobPattern_p.hpp',
        'MimeHandler/NBMimeMagicRule_p.hpp',
        'MimeHandler/NBMimeMagicRuleMatcher_p.hpp',
        'MimeHandler/NBMimeProvider_p.hpp',
        'MimeHandler/NBMimeType.hpp',
        'MimeHandler/NBMimeType_p.hpp',
        'MimeHandler/NBMimeTypeParser_p.hpp',
        'StandardPaths/NBStandardPaths.hpp'
    ]

    Sources += [
    		'MimeHandler/NBMimeDatabase.cpp',
    		'MimeHandler/NBMimeDatabase.cpp',
    		'MimeHandler/NBMimeGlobPattern.cpp',
    		'MimeHandler/NBMimeMagicRule.cpp',
    		'MimeHandler/NBMimeMagicRuleMatcher.cpp',
    		'MimeHandler/NBMimeProvider.cpp',
    		'MimeHandler/NBMimeType.cpp',
    		'MimeHandler/NBMimeTypeParser.cpp',
    		'StandardPaths/NBStandardPaths_unix.cpp',
    		'StandardPaths/NBStandardPaths.cpp'
    ]
endif

Includes = [
    'MimeHandler',
    'StandardPaths'
]

Mocs = Qt.compile_moc(
    headers : Headers,
    dependencies: Deps
)

shared = shared_library(
    TgtName, [ Sources, Mocs ],
		version: meson.project_version(),
		include_directories: Includes,
    dependencies: Deps,
    install: true,
    install_dir: join_paths( get_option( 'libdir' ) )
)

static = static_library(
    TgtName, [ Sources, Mocs ],
		include_directories: Includes,
    dependencies: Deps,
    install: get_option( 'install_static' ),
    install_dir: join_paths( get_option( 'libdir' ) )
)

install_headers(
    'libarchiveqt.h'
)

ArchiveQtShared = declare_dependency(
		link_with: shared,
)

ArchiveQtStatic = declare_dependency(
		link_with: static,
)

## PkgConfig Section
pkgconfig = import( 'pkgconfig' )
pkgconfig.generate(
		shared,
		name: TgtName,
		version: meson.project_version(),
		filebase: TgtName,
		description: 'A Qt based archiving solution with libarchive backend',
		requires: [ QtCore.name(), Zlib.name(), Lzma.name(), Archive.name() ],
    libraries: [ 'bz2' ]
)
