cmake_minimum_required(VERSION 3.25)

project(SimIL
  VERSION 0.4.0
  LANGUAGES CXX
  DESCRIPTION "Simulation Input/Output Library"
)
string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWER)
string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UPPER)

# Prevent in-source builds
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
  message(FATAL_ERROR "In-source builds are not allowed. Please create a separate build directory.")
endif()

# Set default build type if not specified
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
endif()

# Include custom modules
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

# Options
option(BUILD_SHARED_LIBS "Build using shared libraries" ON)
option(BUILD_DOCS "Build documentation" ON)
option(SIMIL_WITH_EXAMPLES "Build examples" ON)
option(SIMIL_WITH_EXPORTERS "Build exporters" ON)
option(SIMIL_WITH_TESTS "Build tests" ON)
option(SIMIL_OPTIONALS_AS_REQUIRED "Force optional dependencies as required" OFF)

option(SIMIL_BRION_ENABLED "Use Brion library to read BlueBrain data structures" OFF)
option(SIMIL_WITH_REST_API "Enable REST API for data streaming input" OFF)

# Dependencies
find_package(Boost REQUIRED CONFIG COMPONENTS)
find_package(HDF5 REQUIRED COMPONENTS C CXX)
find_package(glm REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Core OPTIONAL_COMPONENTS Widgets)

# find_package(Threads)
# find_package(ZeroEQ)
# find_package(Lexis)
# find_package(gmrvlex)

# Optional dependencies
set(SIMIL_OPTS_FIND_ARGS "")
if(SIMIL_OPTIONALS_AS_REQUIRED)
  set(SIMIL_OPTS_FIND_ARGS REQUIRED)
endif()

include(FetchContent)

if(SIMIL_BRION_ENABLED)
  FetchContent_Declare(
      Brion
      GIT_REPOSITORY https://github.com/vg-lab/Brion.git
      GIT_SHALLOW ON
      FIND_PACKAGE_ARGS
  )
  FetchContent_MakeAvailable(Brion)
endif()

# FetchContent_Declare(
#     glm
#     GIT_REPOSITORY https://github.com/g-truc/glm.git
#     GIT_TAG 1.0.1
#     GIT_SHALLOW ON
#     GIT_SUBMODULES_RECURSE OFF
#     FIND_PACKAGE_ARGS
# )
# # set(GLM_BUILD_INSTALL CACHE BOOL "" FORCE ON)
# FetchContent_MakeAvailable(glm)

# Definitions
option(SIMIL_USE_BRION "" ${Brion_FOUND})
# option(SIMIL_USE_ZEROEQ "" ${ZeroEQ_FOUND})
# option(SIMIL_USE_LEXIS "" ${Lexis_FOUND})
# option(SIMIL_USE_GMRVLEX "" ${gmrvlex_FOUND})

# Add subdirectories
add_subdirectory(${PROJECT_NAME_LOWER})

if(Qt5Widgets_FOUND)
  add_subdirectory(qsimil)
endif()

# # Don't build tests when others include the library
# if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND SIMIL_WITH_TESTS)
#   include(CTest)
#   add_subdirectory(tests)
# endif()

# Don't build examples when others include the library
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND SIMIL_WITH_EXAMPLES)
  add_subdirectory(examples)
endif()

# Don't build exporters when others include the library
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND SIMIL_WITH_EXPORTERS)
  add_subdirectory(exporters)
endif()

# Define installation directory variables
include(GNUInstallDirs)

# Include the CMake module that provides helpers for creating package config files
include(CMakePackageConfigHelpers)

# Generate the project-specific cmake config file
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/project-config.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME_LOWER}-config.cmake"
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME_LOWER}
)

# Generate the project-specific cmake config file
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME_LOWER}-config-version.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
)

# Generate the project-specific cmake config version file
install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME_LOWER}-config.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME_LOWER}-config-version.cmake"
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME_LOWER}
)

# Install repo data
install(FILES
  "${CMAKE_CURRENT_SOURCE_DIR}/README.md"
  "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt"
  DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME_LOWER}
)

# Generate documentation
find_package(Doxygen)
if(DOXYGEN_FOUND AND BUILD_DOCS)
  set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/docs_doxygen)
  set(DOXYFILE_IN ${PROJECT_SOURCE_DIR}/cmake/Doxyfile.in)
  set(DOXYFILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

  configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)

  doxygen_add_docs(
    ${PROJECT_NAME}_doxygen
    CONFIG_FILE ${DOXYFILE_OUT}
  )

  install(DIRECTORY
    ${DOXYGEN_OUTPUT_DIR}/html
    DESTINATION ${CMAKE_INSTALL_DOCDIR}
    COMPONENT ${PROJECT_NAME}_documentation
    OPTIONAL
  )
endif()

# Packaging
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE.txt")
set(CPACK_RESOURCE_FILE_README "${PROJECT_SOURCE_DIR}/README.md")
set(CPACK_SOURCE_GENERATOR "TGZ")
include(CPack)
