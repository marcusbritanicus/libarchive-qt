archiver = executable(
    'archiver', [ 'archiver.cpp' ],
    dependencies: [ Deps, ArchiveQtStatic ],
    cpp_args: [ '-DARCHIVEQT_VERSION_STR="v@0@"'.format( meson.project_version() ) ],
    install: true
)
