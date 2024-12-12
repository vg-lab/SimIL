
set( SIMIL_PUBLIC_HEADERS
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
)

set( SIMIL_HEADERS
)

set( SIMIL_SOURCES
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
)

set( SIMIL_LINK_LIBRARIES
     vmmlib
     ${Boost_LIBRARIES}
     ${Boost_SYSTEM_LIBRARY}
     ${HDF5_LIBRARIES}
     Qt5::Core
     Qt5::Widgets
)

if(WIN32)
  configure_file("${PROJECT_SOURCE_DIR}/simil/SimILWinProps.rc.in" "${PROJECT_BINARY_DIR}/SimILWinProps.rc")
  set( SIMIL_SOURCES ${SIMIL_SOURCES} "${PROJECT_BINARY_DIR}/SimILWinProps.rc")
endif(WIN32)

if( BRION_FOUND )
  list( APPEND SIMIL_LINK_LIBRARIES Brion Brain )
  list( APPEND SIMIL_PUBLIC_HEADERS loaders/LoaderBlueConfigData.h )
  list( APPEND SIMIL_SOURCES loaders/LoaderBlueConfigData.cpp )
endif( )

if( SIMIL_WITH_REST_API )
    list( APPEND SIMIL_PUBLIC_HEADERS loaders/LoaderRestData.h
                                      loaders/HTTP/SyncClient.h
                                      loaders/jsoncpp/json/json.h 
                                      loaders/jsoncpp/json/json-forwards.h )
    list( APPEND SIMIL_SOURCES loaders/LoaderRestData.cpp
                               loaders/HTTP/SyncClient.cpp
                               loaders/jsoncpp/jsoncpp.cpp  )
  if(WIN32)
    set( SIMIL_LINK_LIBRARIES pthread ws2_32 ${SIMIL_LINK_LIBRARIES})
  endif(WIN32)
endif()

if( ZEROEQ_FOUND )
  list( APPEND SIMIL_LINK_LIBRARIES ZeroEQ ${CMAKE_THREAD_LIBS_INIT} )
endif( )

if( GMRVLEX_FOUND )
  list( APPEND SIMIL_LINK_LIBRARIES gmrvlex )
endif( )

if( LEXIS_FOUND )
  list( APPEND SIMIL_LINK_LIBRARIES Lexis )
endif( )

add_definitions( -DSIMIL_SHARED )

set( SIMIL_INCLUDE_NAME simil )
set( SIMIL_NAMESPACE simil )

common_library( SimIL )
target_include_directories(SimIL PUBLIC ${HDF5_INCLUDE_DIRS})
