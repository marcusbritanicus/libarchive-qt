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
    link_args: ['-rdynamic','-fPIC'],
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
    libraries: [ '-lbz2' ]
)
