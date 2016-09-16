# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#
#   SimIL
#   2016 (c) GMRV / Universidad Rey Juan Carlos
#   gmrv@gmrv.es
#   www.gmrv.es
#
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
cmake_minimum_required( VERSION 3.1 FATAL_ERROR )
project( SimIL VERSION 0.1.0 )
set( SimIL_VERSION_ABI 1)

# Disable in source building
if( "${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}" )
  message( FATAL "no in source building allowed." )
endif()

list( APPEND CMAKE_MODULE_PATH
  ${PROJECT_SOURCE_DIR}/CMake
  ${CMAKE_SOURCE_DIR}/CMake/common )

# Default build type is Debug
if( NOT CMAKE_BUILD_TYPE )
  set( CMAKE_BUILD_TYPE Debug )
endif( NOT CMAKE_BUILD_TYPE )

if ( CMAKE_BUILD_TYPE MATCHES "Debug" )
  add_definitions( -DDEBUG )
endif( )

include(GitExternal)

set( CMAKECOMMONTEMPLATE_DESCRIPTION "SimIL - Simulation Input/Output Library" )
set( COMMON_PROJECT_DOMAIN gmrv.es )

option( SIMIL_OPTIONALS_AS_REQUIRED
  "Force optional dependencies as required" OFF )

if( SIMIL_OPTIONALS_AS_REQUIRED )
  set( SIMIL_OPTS_FIND_ARGS "REQUIRED" )
else()
  set( SIMIL_OPTS_FIND_ARGS "" )
endif()

include( Common )

common_find_package( Brion ${SIMIL_OPTS_FIND_ARGS} )
common_find_package( HDF5 REQUIRED COMPONENTS C CXX )
common_find_package( Boost REQUIRED )
common_find_package( ZeroEQ )
common_find_package( Qt5Core 5.4 SYSTEM )
common_find_package( Qt5Widgets 5.4 SYSTEM )

list( APPEND SIMIL_DEPENDENT_LIBRARIES HDF5 Boost )

if( BRION_FOUND )
  list( APPEND SIMIL_DEPENDENT_LIBRARIES Brion )
endif()

if( ZEROEQ_FOUND )

  list(APPEND SIMIL_DEPENDENT_LIBRARIES ZeroEQ)

  common_find_package(Threads REQUIRED)
  list(APPEND SIMIL_DEPENDENT_LIBRARIES Threads)
  
  common_find_package(Lexis)
  if(LEXIS_FOUND)
    list(APPEND SIMIL_DEPENDENT_LIBRARIES Lexis)
  endif()
      
  common_find_package( gmrvlex )
  if ( GMRVLEX_FOUND )
    list(APPEND SIMIL_DEPENDENT_LIBRARIES gmrvlex )
  endif( )  
  
endif( )

common_find_package_post( )

add_subdirectory( simil )
if( QT5CORE_FOUND)
  add_subdirectory( qsimil )
endif()
add_subdirectory( examples )
# add_subdirectory( tests )

include( CPackConfig )
include( DoxygenRule )
