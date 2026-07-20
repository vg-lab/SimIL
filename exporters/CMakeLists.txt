if(SIMIL_WITH_EXPORTERS)

  # Function to create and install an exporter application
  function(add_project_exporter)
    set(options)
    set(oneValueArgs TARGET_NAME)
    set(multiValueArgs SOURCES LINK_LIBRARIES COMPILE_OPTIONS)
    cmake_parse_arguments(PARSE_ARGV 0 ARG "${options}" "${oneValueArgs}" "${multiValueArgs}")

    if(NOT ARG_TARGET_NAME OR NOT ARG_SOURCES)
      message(FATAL_ERROR "TARGET_NAME and SOURCES are required arguments")
    endif()

    add_executable(${ARG_TARGET_NAME} ${ARG_SOURCES})
    target_compile_definitions(${ARG_TARGET_NAME} PRIVATE ${ARG_COMPILE_OPTIONS})
    target_link_libraries(${ARG_TARGET_NAME} PRIVATE ${ARG_LINK_LIBRARIES})

    # Get current parent scope value and create a local copy
    set(PROJECT_EXPORTERS_FILES_VAR "${${PROJECT_NAME}_EXPORTERS_FILES}")
    set(current_exporters ${PROJECT_EXPORTERS_FILES_VAR})
    
    # Convert to CMake list and append new sources
    list(APPEND current_exporters ${ARG_SOURCES})
    
    # Remove duplicates and update parent scope
    list(REMOVE_DUPLICATES current_exporters)
    set(${PROJECT_NAME}_EXPORTERS_FILES "${current_exporters}" PARENT_SCOPE)

    # Create all exporters target
    if(NOT TARGET ${PROJECT_NAME}_all_exporters)
      add_custom_target(${PROJECT_NAME}_all_exporters)
    endif()

    # Add exporter to global exporters target
    add_dependencies(${PROJECT_NAME}_all_exporters ${ARG_TARGET_NAME})
  endfunction()

  add_project_exporter(
    TARGET_NAME hdf52HANOIExporter
    SOURCES
      hdf52hanoiExporter.cpp
    LINK_LIBRARIES
      HDF5::HDF5
  )

  # Define installation directory variables
  include(GNUInstallDirs)

  # Install exporters source files
  install(FILES ${${PROJECT_NAME}_EXPORTERS_FILES}
    DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME_LOWER}/exporters
    COMPONENT ${PROJECT_NAME}_exporters
  )

endif()
