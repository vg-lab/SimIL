# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#
#   SimIL
#   2016 (c) VG-Lab / Universidad Rey Juan Carlos
#   www.vg-lab.es
#
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
cmake_minimum_required( VERSION 3.1 FATAL_ERROR )
project( SimIL VERSION 0.3.4 )
set( SimIL_VERSION_ABI 2)

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
set( COMMON_PROJECT_DOMAIN es.vg-lab )

option( SIMIL_BRION_ENABLED
  "Use Brion library to read BlueBrain data structures" OFF )

option( SIMIL_WITH_REST_API
  "Enable REST API for data streaming input" OFF )
  
include( Common )

common_find_package( vmmlib REQUIRED SYSTEM )
if( SIMIL_BRION_ENABLED )
  common_find_package( Brion REQUIRED )
endif()
common_find_package( HDF5 REQUIRED COMPONENTS C CXX SYSTEM )
common_find_package( Boost REQUIRED COMPONENTS system )
common_find_package( ZeroEQ )
common_find_package( Qt5Core SYSTEM )
common_find_package( Qt5Widgets SYSTEM )

list( APPEND SIMIL_DEPENDENT_LIBRARIES HDF5 vmmlib Boost )

if( BRION_FOUND )
  list( APPEND SIMIL_DEPENDENT_LIBRARIES Brion )
endif()

if( ZEROEQ_FOUND )

  list( APPEND SIMIL_DEPENDENT_LIBRARIES ZeroEQ )

  common_find_package( Threads REQUIRED )
  list( APPEND SIMIL_DEPENDENT_LIBRARIES Threads )

  common_find_package( Lexis )
  if( LEXIS_FOUND )
    list( APPEND SIMIL_DEPENDENT_LIBRARIES Lexis )
  endif( )

  common_find_package( gmrvlex )
  if( GMRVLEX_FOUND )
    list( APPEND SIMIL_DEPENDENT_LIBRARIES gmrvlex )
  endif( )

endif( )

common_find_package_post( )

add_subdirectory( simil )

if( QT5CORE_FOUND )
  add_subdirectory( qsimil )
endif()

add_subdirectory( examples )

include( CPackConfig )
include( DoxygenRule )
