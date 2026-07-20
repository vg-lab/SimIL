# Set the target name
set(CURRENT_TARGET_NAME SimIL)
string(TOLOWER ${CURRENT_TARGET_NAME} CURRENT_TARGET_NAME_LOWER)
string(TOUPPER ${CURRENT_TARGET_NAME} CURRENT_TARGET_NAME_UPPER)

# Define public headers
set(SIMIL_PUBLIC_HEADERS
  types.h
  log.h
  SimulationPlayer.h
  SpikesPlayer.h
  VoltagesPlayer.h
  DataSet.h
  SimulationData.h
  SpikeData.h
  VoltageData.h
  Network.h
  ZeroEqEventsManager.h
  SubsetEventManager.h
  Spikes.hpp
  loaders/LoaderSimData.h

  loaders/LoaderHDF5Data.h
  loaders/auxiliar/H5Network.h
  loaders/auxiliar/H5Activity.h
  loaders/auxiliar/H5SubsetEvents.h

  loaders/LoaderCSVData.h
  loaders/auxiliar/CSVNetwork.h
  loaders/auxiliar/CSVActivity.h
  loaders/auxiliar/H5Morphologies.h

  loaders/LoaderSnuddaData.h
)

# Define source files
set(SIMIL_SOURCES
  SimulationPlayer.cpp
  SpikesPlayer.cpp
  VoltagesPlayer.cpp
  DataSet.cpp
  SimulationData.cpp
  SpikeData.cpp
  VoltageData.cpp
  Network.cpp

  ZeroEqEventsManager.cpp
  SubsetEventManager.cpp

  loaders/LoaderHDF5Data.cpp
  loaders/auxiliar/H5Network.cpp
  loaders/auxiliar/H5Activity.cpp
  loaders/auxiliar/H5SubsetEvents.cpp

  loaders/LoaderCSVData.cpp
  loaders/auxiliar/CSVNetwork.cpp
  loaders/auxiliar/CSVActivity.cpp
  loaders/auxiliar/H5Morphologies.cpp

  loaders/LoaderSnuddaData.cpp
)

if(SIMIL_USE_BRION)
  list(APPEND SIMIL_PUBLIC_HEADERS loaders/LoaderBlueConfigData.h)
  list(APPEND SIMIL_SOURCES loaders/LoaderBlueConfigData.cpp)
endif()

if(SIMIL_WITH_REST_API)
  list(APPEND SIMIL_PUBLIC_HEADERS
    loaders/LoaderRestData.h
    loaders/HTTP/SyncClient.h
    loaders/jsoncpp/json/json.h
    loaders/jsoncpp/json/json-forwards.h
  )
  list(APPEND SIMIL_SOURCES
    loaders/LoaderRestData.cpp
    loaders/HTTP/SyncClient.cpp
    loaders/jsoncpp/jsoncpp.cpp
  )
endif()

if(WIN32)
  configure_file(
    "${PROJECT_SOURCE_DIR}/${CURRENT_TARGET_NAME_LOWER}/SimILWinProps.rc.in"
    "${PROJECT_BINARY_DIR}/SimILWinProps.rc"
  )
  set(SIMIL_SOURCES ${SIMIL_SOURCES} "${PROJECT_BINARY_DIR}/SimILWinProps.rc")
endif()

# Create the includes strings for the unified header
set(HEADER_INCLUDES "")
foreach(header ${SIMIL_PUBLIC_HEADERS})
  string(APPEND HEADER_INCLUDES "#include <${CURRENT_TARGET_NAME_LOWER}/${header}>\n")
endforeach()

# Generate the unified header
configure_file(
  "${PROJECT_SOURCE_DIR}/cmake/unified_header-component.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/${CURRENT_TARGET_NAME_LOWER}.h"
)

# Get the current Git commit for the version header
find_package(Git)
if(Git_FOUND)
  execute_process(
    COMMAND ${GIT_EXECUTABLE} --no-pager log -n 1 HEAD "--pretty=format:%h"
    OUTPUT_VARIABLE GIT_COMMIT_HASH
    RESULT_VARIABLE git_result
    ERROR_QUIET
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  if(git_result EQUAL 0)
    message(STATUS "Last ${PROJECT_NAME}::${CURRENT_TARGET_NAME} Git commit: ${GIT_COMMIT_HASH}")
  endif()
endif()

# Generate the version header
configure_file(
  "${PROJECT_SOURCE_DIR}/cmake/version-component.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/version.h"
)

# Create the target
add_library(${CURRENT_TARGET_NAME})
add_library(${PROJECT_NAME}::${CURRENT_TARGET_NAME} ALIAS ${CURRENT_TARGET_NAME})

set_target_properties(${CURRENT_TARGET_NAME} PROPERTIES
  VERSION ${PROJECT_VERSION}
  SOVERSION ${PROJECT_VERSION_MAJOR}
)

# Generate export header
include(GenerateExportHeader)
generate_export_header(${CURRENT_TARGET_NAME})

# Add sources to the target
target_sources(${CURRENT_TARGET_NAME}
  PRIVATE
    ${SIMIL_SOURCES}
  PUBLIC
    FILE_SET public_headers
    TYPE HEADERS
    BASE_DIRS
      ${PROJECT_SOURCE_DIR}
    FILES
      ${SIMIL_PUBLIC_HEADERS}
)

# Add generated sources to the target
target_sources(${CURRENT_TARGET_NAME}
  PUBLIC
    FILE_SET public_headers_generated
    TYPE HEADERS
    BASE_DIRS
      ${PROJECT_BINARY_DIR}
    FILES
      ${CMAKE_CURRENT_BINARY_DIR}/${CURRENT_TARGET_NAME_LOWER}.h
      ${CMAKE_CURRENT_BINARY_DIR}/${CURRENT_TARGET_NAME_LOWER}_export.h
      ${CMAKE_CURRENT_BINARY_DIR}/version.h
)

# Add compile definitions
target_compile_definitions(${CURRENT_TARGET_NAME}
  PUBLIC
    $<$<BOOL:${SIMIL_USE_BRION}>:SIMIL_USE_BRION>
    $<$<BOOL:${SIMIL_USE_ZEROEQ}>:SIMIL_USE_ZEROEQ>
    $<$<BOOL:${SIMIL_USE_LEXIS}>:SIMIL_USE_LEXIS>
    $<$<BOOL:${SIMIL_USE_GMRVLEX}>:SIMIL_USE_GMRVLEX>
  PRIVATE
    $<$<STREQUAL:${CMAKE_BUILD_TYPE},Debug>:DEBUG>
)

# Link libraries
target_link_libraries(${CURRENT_TARGET_NAME}
  PUBLIC
    glm::glm
    HDF5::HDF5
    Qt5::Core
    "$<$<BOOL:${SIMIL_USE_BRION}>:Brain>"
    "$<$<BOOL:${SIMIL_USE_BRION}>:Brion>"
    "$<$<BOOL:${SIMIL_USE_ZEROEQ}>:ZeroEQ>"
    "$<$<BOOL:${SIMIL_USE_LEXIS}>:Lexis>"
    "$<$<BOOL:${SIMIL_USE_GMRVLEX}>:gmrvlex::gmrvlex>"
  PRIVATE
)

# # Don't build tests when others include the library
# if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND SIMIL_WITH_TESTS)
#   include(CTest)
#   add_subdirectory(tests)
# endif()

# Make cache variables for install destinations
include(GNUInstallDirs)

# Create an export
install(TARGETS ${CURRENT_TARGET_NAME}
  EXPORT ${CURRENT_TARGET_NAME}Targets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${CURRENT_TARGET_NAME_LOWER}
  FILE_SET public_headers DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  FILE_SET public_headers_generated DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Install the project targets file
install(EXPORT ${CURRENT_TARGET_NAME}Targets
  FILE ${CURRENT_TARGET_NAME_LOWER}-targets.cmake
  NAMESPACE ${PROJECT_NAME}::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME_LOWER}/${CURRENT_TARGET_NAME_LOWER}
  COMPONENT ${CURRENT_TARGET_NAME}
)

# Include the CMake module that provides helpers for creating package config files
include(CMakePackageConfigHelpers)

# Generate the project-specific cmake config file
configure_package_config_file(${PROJECT_SOURCE_DIR}/cmake/project-component-config.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/${CURRENT_TARGET_NAME_LOWER}-config.cmake"
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME_LOWER}/${CURRENT_TARGET_NAME_LOWER}
)

# Generate the project-specific cmake config version file
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/${CURRENT_TARGET_NAME_LOWER}-config-version.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
)

# Install the cmake config files
install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/${CURRENT_TARGET_NAME_LOWER}-config.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/${CURRENT_TARGET_NAME_LOWER}-config-version.cmake"
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME_LOWER}/${CURRENT_TARGET_NAME_LOWER}
)

# Allow the configured project-config.cmake in the build directory to be used by
# other projects, without needing it to be installed
export(EXPORT ${CURRENT_TARGET_NAME}Targets
  NAMESPACE ${PROJECT_NAME}::
  FILE ${CURRENT_TARGET_NAME_LOWER}-targets.cmake
)
