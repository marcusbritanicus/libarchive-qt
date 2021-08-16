cmake_policy(SET CMP0048 NEW)
project( ArchiveQt VERSION 2.0.6 )

cmake_minimum_required( VERSION 3.8 )

# set( PROJECT_VERSION 2.0.6 CACHE INTERNAL "" )
# set( PROJECT_VERSION_MAJOR 2 CACHE INTERNAL "" )
# set( PROJECT_VERSION_MINOR 0 CACHE INTERNAL "" )
# set( PROJECT_VERSION_PATCH 6 CACHE INTERNAL "" )

set( PROJECT_VERSION_MAJOR_MINOR ${CMAKE_PROJECT_VERSION_MAJOR}.${CMAKE_PROJECT_VERSION_MINOR} )
add_compile_definitions(ARCHIVEQT_VERSION_STR="${CMAKE_PROJECT_VERSION}")

add_definitions ( -Wall )
if ( CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT )
    set( CMAKE_INSTALL_PREFIX "/usr" CACHE PATH "Location for installing the project" FORCE )
endif()

set( USEQT "Qt5" CACHE STRING "Build using particular version of Qt" )
set( INSTALL_STATIC TRUE CACHE BOOL "Install the static library as well" )

if ( USEQT STREQUAL "Qt4" OR USEQT STREQUAL "4" )
    set( USEQT "Qt4" )
elseif ( USEQT STREQUAL "Qt5" OR USEQT STREQUAL "5" )
    set( USEQT "Qt5" )
elseif ( USEQT STREQUAL "Qt6" OR USEQT STREQUAL "6" )
    set( USEQT "Qt6" )
else()
    message( FATAL_ERROR "Unsupported version of Qt specified. Choose between Qt4, Qt5 and Qt6." )
endif()

include_directories( lib )

add_subdirectory( lib )
add_subdirectory( archiver )

add_dependencies( archiver mylib_static )

message( "Installing to ${CMAKE_INSTALL_PREFIX}" )
