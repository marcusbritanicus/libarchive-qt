project( archiveqt )
cmake_minimum_required( VERSION 3.1 )

set( PROJECT_VERSION 2.0.1 )
set( PROJECT_VERSION_MAJOR 2 )
set( PROJECT_VERSION_MINOR 0 )
set( PROJECT_VERSION_PATCH 1 )

set( PROJECT_VERSION_MAJOR_MINOR ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR} )

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
	include_directories(
	    ${CMAKE_CURRENT_SOURCE_DIR}
	    ${CMAKE_CURRENT_BINARY_DIR}
	    ${QT_INCLUDE_DIR}
	    ${QT_QTCORE_INCLUDE_DIR}
    )
elseif ( USEQT STREQUAL "Qt5" )
	find_package( Qt5Core REQUIRED )
elseif ( USEQT STREQUAL "Qt6" )
	find_package( Qt6Core REQUIRED )
endif()

set( archiveqt_HDRS
	LibLzma.hpp
	LibLzma2.hpp
	LibBZip2.hpp
	LibGZip.hpp
	lz4dec.h
	libarchiveqt.h
)

set( archiveqt_SRCS
	LibArchive.cpp
	LibLzma.cpp
	LibLzma2.cpp
	LibBZip2.cpp
	LibGZip.cpp
	lz4dec.c
	LibLZip.cpp
)

# if ( USE_LZLIB )
#     message( "Compiling with lzlib support ${USE_LZLIB}" )
#     message( "Including LibLZip sources" )
#     list(
# 		APPEND
# 		archiveqt_HDRS
# 		LibLZip.hpp
# 	)
#     list(
# 		APPEND
# 		archiveqt_SRCS
# 		LibLZip.cpp
# 	)
# endif()

if ( USEQT STREQUAL "Qt4" OR USEQT STREQUAL "4" )
	include_directories( MimeHandler StandardPaths )
	list(
		APPEND
		archiveqt_HDRS
		MimeHandler/NBMimeDatabase.hpp
		MimeHandler/NBMimeDatabase_p.hpp
		MimeHandler/NBMimeGlobPattern_p.hpp
		MimeHandler/NBMimeMagicRule_p.hpp
		MimeHandler/NBMimeMagicRuleMatcher_p.hpp
		MimeHandler/NBMimeProvider_p.hpp
		MimeHandler/NBMimeType.hpp
		MimeHandler/NBMimeType_p.hpp
		MimeHandler/NBMimeTypeParser_p.hpp
		StandardPaths/NBStandardPaths.hpp
	)

	list(
		APPEND
		archiveqt_SRCS
		MimeHandler/NBMimeDatabase.cpp
		MimeHandler/NBMimeDatabase.cpp
		MimeHandler/NBMimeGlobPattern.cpp
		MimeHandler/NBMimeMagicRule.cpp
		MimeHandler/NBMimeMagicRuleMatcher.cpp
		MimeHandler/NBMimeProvider.cpp
		MimeHandler/NBMimeType.cpp
		MimeHandler/NBMimeTypeParser.cpp
		StandardPaths/NBStandardPaths_unix.cpp
		StandardPaths/NBStandardPaths.cpp
	)
endif()

add_library( archiveqt OBJECT ${archiveqt_SRCS} ${archiveqt_HDRS} )
set_property( TARGET archiveqt PROPERTY POSITION_INDEPENDENT_CODE ON )

if ( USEQT STREQUAL "Qt4" )
	target_include_directories( archiveqt PRIVATE $<TARGET_PROPERTY:Qt4::QtCore,INTERFACE_INCLUDE_DIRECTORIES>)
	target_compile_definitions( archiveqt PRIVATE $<TARGET_PROPERTY:Qt4::QtCore,INTERFACE_COMPILE_DEFINITIONS>)
elseif ( USEQT STREQUAL "Qt5" )
	target_include_directories( archiveqt PRIVATE $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>)
	target_compile_definitions( archiveqt PRIVATE $<TARGET_PROPERTY:Qt5::Core,INTERFACE_COMPILE_DEFINITIONS>)
elseif ( USEQT STREQUAL "Qt6" )
	target_include_directories( archiveqt PRIVATE $<TARGET_PROPERTY:Qt::Core,INTERFACE_INCLUDE_DIRECTORIES>)
	target_compile_definitions( archiveqt PRIVATE $<TARGET_PROPERTY:Qt::Core,INTERFACE_COMPILE_DEFINITIONS>)
endif()

add_library( mylib_shared SHARED $<TARGET_OBJECTS:archiveqt> )
set_target_properties( mylib_shared PROPERTIES VERSION ${PROJECT_VERSION} )
set_target_properties( mylib_shared PROPERTIES SOVERSION ${PROJECT_VERSION} )
set_target_properties( mylib_shared PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR_MINOR} )
set_target_properties( mylib_shared PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR} )

if ( USEQT STREQUAL "Qt4" )
	set_target_properties( mylib_shared PROPERTIES OUTPUT_NAME archiveqt4 )
	target_link_libraries( mylib_shared  Qt4::QtCore archive z bz2 lzma )
elseif ( USEQT STREQUAL "Qt5" )
	set_target_properties( mylib_shared PROPERTIES OUTPUT_NAME archiveqt5 )
	target_link_libraries( mylib_shared  Qt5::Core archive z bz2 lzma )
elseif ( USEQT STREQUAL "Qt6" )
	set_target_properties( mylib_shared PROPERTIES OUTPUT_NAME archiveqt6 )
	target_link_libraries( mylib_shared  Qt::Core archive z bz2 lzma )
endif()


add_library( mylib_static STATIC $<TARGET_OBJECTS:archiveqt> )
if ( USEQT STREQUAL "Qt4" )
	set_target_properties( mylib_static PROPERTIES OUTPUT_NAME archiveqt4 )
	target_link_libraries( mylib_static  Qt4::QtCore archive z bz2 lzma )
elseif ( USEQT STREQUAL "Qt5" )
	set_target_properties( mylib_static PROPERTIES OUTPUT_NAME archiveqt5 )
	target_link_libraries( mylib_static  Qt5::Core archive z bz2 lzma )
elseif ( USEQT STREQUAL "Qt6" )
	set_target_properties( mylib_static PROPERTIES OUTPUT_NAME archiveqt6 )
	target_link_libraries( mylib_static  Qt::Core archive z bz2 lzma )
endif()

install( TARGETS mylib_shared EXPORT LIBRARY )
install( TARGETS mylib_static EXPORT LIBRARY )
install( FILES libarchiveqt.h DESTINATION include/ )
