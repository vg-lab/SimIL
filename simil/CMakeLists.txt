
set( SIMIL_PUBLIC_HEADERS
     types.h
     log.h
     SimulationPlayer.h
     SpikesPlayer.h
     DataSet.h
     SimulationData.h
     SpikeData.h
     Network.h
     ZeroEqEventsManager.h
     SubsetEventManager.h
     Spikes.hpp
     loaders/LoaderSimData.h

     loaders/LoaderHDF5Data.h
     loaders/aux/H5Network.h
     loaders/aux/H5Activity.h
     loaders/aux/H5SubsetEvents.h

     loaders/LoaderCSVData.h
     loaders/aux/CSVNetwork.h
     loaders/aux/CSVActivity.h
)

set( SIMIL_HEADERS
)

set( SIMIL_SOURCES
     SimulationPlayer.cpp
     SpikesPlayer.cpp
     DataSet.cpp
     SimulationData.cpp
     SpikeData.cpp
     Network.cpp

     ZeroEqEventsManager.cpp
     SubsetEventManager.cpp

     loaders/LoaderHDF5Data.cpp
     loaders/aux/H5Network.cpp
     loaders/aux/H5Activity.cpp
     loaders/aux/H5SubsetEvents.cpp

     loaders/LoaderCSVData.cpp
     loaders/aux/CSVNetwork.cpp
     loaders/aux/CSVActivity.cpp

)

set( SIMIL_LINK_LIBRARIES
     vmmlib
     ${Boost_LIBRARIES}
     ${Boost_SYSTEM_LIBRARY}
     ${HDF5_LIBRARIES}
     Qt5::Core
     Qt5::Widgets
)

if( BRION_FOUND )
  list( APPEND SIMIL_LINK_LIBRARIES Brion Brain )
  list( APPEND SIMIL_PUBLIC_HEADERS loaders/LoaderBlueConfigData.h )
  list( APPEND SIMIL_SOURCES loaders/LoaderBlueConfigData.cpp )
endif( )

if( SIMIL_WITH_REST_API )
    list( APPEND SIMIL_PUBLIC_HEADERS loaders/LoaderRestData.h
                                      loaders/HTTP/SyncClient.h )
    list( APPEND SIMIL_SOURCES loaders/LoaderRestData.cpp
                               loaders/HTTP/SyncClient.cpp )
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
