if(SIMIL_WITH_EXAMPLES)

  # Function to create and install an example application
  function(add_project_example)
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
    set(PROJECT_EXAMPLES_FILES_VAR "${${PROJECT_NAME}_EXAMPLES_FILES}")
    set(current_examples ${PROJECT_EXAMPLES_FILES_VAR})
    
    # Convert to CMake list and append new sources
    list(APPEND current_examples ${ARG_SOURCES})
    
    # Remove duplicates and update parent scope
    list(REMOVE_DUPLICATES current_examples)
    set(${PROJECT_NAME}_EXAMPLES_FILES "${current_examples}" PARENT_SCOPE)

    # Create all examples target
    if(NOT TARGET ${PROJECT_NAME}_all_examples)
      add_custom_target(${PROJECT_NAME}_all_examples)
    endif()

    # Add example to global examples target
    add_dependencies(${PROJECT_NAME}_all_examples ${ARG_TARGET_NAME})
  endfunction()

  add_project_example(
    TARGET_NAME similRefactorLoadExample
    SOURCES
      loadRefactorExample.cpp
    LINK_LIBRARIES
      SimIL::SimIL
    COMPILE_OPTIONS
      $<$<BOOL:${SIMIL_USE_BRION}>:SIMIL_USE_BRION>
  )

  add_project_example(
    TARGET_NAME similCSVExample
    SOURCES
      csv.cpp
    LINK_LIBRARIES
      SimIL::SimIL
  )

  add_project_example(
    TARGET_NAME similLoadExample
    SOURCES
      loadExample.cpp
    LINK_LIBRARIES
      SimIL::SimIL
  )

  add_project_example(
    TARGET_NAME similSubset
    SOURCES
      subset.cpp
    LINK_LIBRARIES
      SimIL::SimIL
  )

  add_project_example(
    TARGET_NAME similBcToCsv
    SOURCES
      bcToCsv.cpp
    LINK_LIBRARIES
      SimIL::SimIL
  )

  if(SIMIL_WITH_REST_API)
    add_project_example(
      TARGET_NAME similRestAPI
      SOURCES
        RESTExample.cpp
      LINK_LIBRARIES
        SimIL::SimIL
    )
  endif()

  # Define installation directory variables
  include(GNUInstallDirs)

  # Install examples source files
  install(FILES ${${PROJECT_NAME}_EXAMPLES_FILES}
    DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME_LOWER}/examples
    COMPONENT ${PROJECT_NAME}_examples
  )

endif()