project( archiver )
cmake_minimum_required( VERSION 3.8 )

set( PROJECT_VERSION 2.0.4 )
set( PROJECT_VERSION_MAJOR 2 )
set( PROJECT_VERSION_MINOR 0 )
set( PROJECT_VERSION_PATCH 4 )

set( PROJECT_VERSION_MAJOR_MINOR ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR} )
add_compile_definitions(VERSION_TEXT="${PROJECT_VERSION}")

set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_INCLUDE_CURRENT_DIR ON )
set( CMAKE_BUILD_TYPE Release )

add_definitions ( -Wall )
if ( CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT )
    set( CMAKE_INSTALL_PREFIX "/usr" CACHE PATH "Location for installing the project" FORCE )
endif()

set( CMAKE_AUTOMOC ON )
set( CMAKE_AUTORCC ON )
set( CMAKE_AUTOUIC ON )

if ( USEQT STREQUAL "Qt4" )
	find_package( Qt4 REQUIRED QtCore )
elseif ( USEQT STREQUAL "Qt5" )
	find_package( Qt5Core REQUIRED )
elseif ( USEQT STREQUAL "Qt6" )
	find_package( Qt6Core REQUIRED )
endif()

set( archiver_SRCS
	example.cpp
)

add_executable( archiver ${archiver_SRCS}  )

if ( USEQT STREQUAL "Qt4" )
	target_link_libraries( archiver  Qt4::QtCore ${CMAKE_CURRENT_BINARY_DIR}/../lib/libarchiveqt4.a archive z bz2 lzma )
elseif ( USEQT STREQUAL "Qt5" )
	target_link_libraries( archiver  Qt5::Core ${CMAKE_CURRENT_BINARY_DIR}/../lib/libarchiveqt5.a archive z bz2 lzma )
elseif ( USEQT STREQUAL "Qt6" )
	target_link_libraries( archiver  Qt::Core ${CMAKE_CURRENT_BINARY_DIR}/../lib/libarchiveqt6.a archive z bz2 lzma )
endif()


install( TARGETS archiver DESTINATION bin )
