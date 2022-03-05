project(
	'Archive Qt',
    'c',
	'cpp',
	version: '2.0.7',
	license: 'GPLv3',
	meson_version: '>=0.59.0',
	default_options: [
		'cpp_std=c++17',
        'c_std=c11',
		'warning_level=2',
		'werror=false',
	],
)

add_global_arguments( '-DPROJECT_VERSION=' + meson.project_version(), language : 'cpp' )
add_global_arguments( '-fPIC', language : 'cpp' )

Zlib = dependency( 'zlib' )
Lzma = dependency( 'liblzma' )
BZip = meson.get_compiler( 'cpp' ).find_library( 'bz2' )
Archive = dependency( 'libarchive' )

if get_option('use_qt_version') == 'qt4'
		Qt = import( 'qt4' )
		QtCore = dependency( 'Qt4Core' )
		TgtName = 'archiveqt4'

elif get_option('use_qt_version') == 'qt5'
		Qt = import( 'qt5' )
		QtCore = dependency( 'Qt5Core' )
		TgtName = 'archiveqt5'

elif get_option('use_qt_version') == 'qt6'
		Qt = import( 'qt6' )
		QtCore = dependency( 'Qt6Core' )
		TgtName = 'archiveqt6'

endif

Deps = [ QtCore, Zlib, Lzma, BZip, Archive ]

Includes = include_directories(
    'lib',
    'lib/MimeHandler',
    'lib/StandardPaths',
)

# Contains the main library
subdir( 'lib' )

# Contains the archvier cli app
subdir( 'archiver' )


summary = [
	'',
	'----------------',
	'Archive Qt @0@'.format(meson.project_version()),
	'----------------',
	''
]
message('\n'.join(summary))
