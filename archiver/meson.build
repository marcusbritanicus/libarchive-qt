archiver = executable(
    'archiver', [ 'archiver.cpp' ],
    dependencies: [ Deps, ArchiveQtStatic ],
    include_directories: Includes,
    cpp_args: [ '-DARCHIVEQT_VERSION_STR="v@0@"'.format( meson.project_version() ) ],
    install: true
)
