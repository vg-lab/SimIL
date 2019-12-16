
set( SIMIL_PUBLIC_HEADERS
     types.h
     log.h
     SimulationPlayer.h
     DataSet.h
     SimulationData.h
     Network.h
     H5Network.h
     H5Activity.h
     CSVNetwork.h
     CSVActivity.h
     ZeroEqEventsManager.h
     SubsetEventManager.h
     H5SubsetEvents.h
     Spikes.hpp
     loaders/LoaderSimData.h
     loaders/LoaderHDF5Data.h
     loaders/LoaderCSVData.h
)

set( SIMIL_HEADERS
)

set( SIMIL_SOURCES
     SimulationPlayer.cpp
     DataSet.cpp
     SimulationData.cpp
     Network.cpp
     H5Network.cpp
     H5Activity.cpp
     CSVNetwork.cpp
     CSVActivity.cpp
     ZeroEqEventsManager.cpp
     SubsetEventManager.cpp
     H5SubsetEvents.cpp
     loaders/LoaderHDF5Data.cpp
    loaders/LoaderCSVData.cpp

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
  list (APPEND SIMIL_PUBLIC_HEADERS loaders/LoaderBlueConfigData.h)
  list (APPEND SIMIL_SOURCES loaders/LoaderBlueConfigData.cpp)
endif( )

if (SIMIL_USE_CONE)
    list( APPEND SIMIL_LINK_LIBRARIES cone::cone)
    list (APPEND SIMIL_PUBLIC_HEADERS loaders/LoaderInsituData.h)
    list (APPEND SIMIL_SOURCES loaders/LoaderInsituData.cpp)
endif()


if (SIMIL_USE_REST_API)

    list (APPEND SIMIL_PUBLIC_HEADERS loaders/LoaderRestData.h
                                      loaders/HTTP/SyncClient.h
                                      )
    list (APPEND SIMIL_SOURCES loaders/LoaderRestData.cpp
                                loaders/HTTP/SyncClient.cpp)
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
