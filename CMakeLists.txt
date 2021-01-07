project( ArchiveQt )
cmake_minimum_required( VERSION 3.1 )

add_definitions ( -Wall )
if ( CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT )
    set( CMAKE_INSTALL_PREFIX "/usr" CACHE PATH "Location for installing the project" FORCE )
endif()

option( USE_LZLIB "Have lzlib installed in your system" OFF )
set( USEQT "Qt5" CACHE STRING "Build using particular version of Qt" )

if ( USEQT STREQUAL "Qt4" OR USEQT STREQUAL "4" )
    set( USEQT "Qt4" )
elseif ( USEQT STREQUAL "Qt5" OR USEQT STREQUAL "5" )
    set( USEQT "Qt5" )
elseif ( USEQT STREQUAL "Qt6" OR USEQT STREQUAL "6" )
    set( USEQT "Qt6" )
else()
    message( FATAL_ERROR "Unsupported version of Qt specified. Choose between Qt4, Qt5 and Qt6." )
endif()

add_subdirectory( lib )
add_subdirectory( example )

add_dependencies( archiver mylib_static )

message( "Installing to ${CMAKE_INSTALL_PREFIX}" )
