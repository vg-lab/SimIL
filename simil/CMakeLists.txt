
set( SIMIL_PUBLIC_HEADERS
     types.h
     log.h
     SimulationPlayer.h
     H5Network.h
     H5Activity.h
     CSVNetwork.h
     CSVActivity.h
     SimulationData.h
     ZeroEqEventsManager.h
     SubsetEventManager.h
     H5SubsetEvents.h
     Spikes.hpp
     loaders/LoadSimData.h
     loaders/LoadHDF5Data.h
     loaders/LoadCSVData.h
)

set( SIMIL_HEADERS
)

set( SIMIL_SOURCES
     SimulationPlayer.cpp
     H5Network.cpp
     H5Activity.cpp
     CSVNetwork.cpp
     CSVActivity.cpp
     SimulationData.cpp
     ZeroEqEventsManager.cpp
     SubsetEventManager.cpp
     H5SubsetEvents.cpp
     loaders/LoadHDF5Data.cpp
    loaders/LoadCSVData.cpp

)

set( SIMIL_LINK_LIBRARIES
     vmmlib
     ${Boost_LIBRARIES}
     ${HDF5_LIBRARIES}
     Qt5::Core
     Qt5::Widgets
)

if( BRION_FOUND )
  list( APPEND SIMIL_LINK_LIBRARIES Brion Brain )
  list (APPEND SIMIL_PUBLIC_HEADERS loaders/LoadblueConfigData.h)
  list (APPEND SIMIL_SOURCES loaders/LoadblueConfigData.cpp)
endif( )

if (SIMIL_USE_CONE)
    #message("Adding Cone to Simil")
    list( APPEND SIMIL_LINK_LIBRARIES cone::cone)

    list (APPEND SIMIL_PUBLIC_HEADERS loaders/LoadInsituData.h)
    list (APPEND SIMIL_SOURCES loaders/LoadInsituData.cpp)
endif()


if (SIMIL_USE_REST_API)

    list (APPEND SIMIL_PUBLIC_HEADERS loaders/LoadRestApiData.h)
    list (APPEND SIMIL_SOURCES loaders/LoadRestApiData.cpp
                                loaders/HTTPClient.hpp)
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
