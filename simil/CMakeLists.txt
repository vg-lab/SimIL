
set( SIMIL_PUBLIC_HEADERS
     types.h
     log.h
     SimulationPlayer.h
     H5Network.h
     H5Activity.h
     SimulationData.h
     ZeroEqEventsManager.h
     SubsetEventManager.h
     H5SubsetEvents.h
     CorrelationComputer.h
     Spikes.hpp
     loaders/LoadSimData.h
     loaders/LoadHDF5Data.h

)

set( SIMIL_HEADERS
)

set( SIMIL_SOURCES
     SimulationPlayer.cpp
     H5Network.cpp
     H5Activity.cpp
     SimulationData.cpp
     ZeroEqEventsManager.cpp
     SubsetEventManager.cpp
     H5SubsetEvents.cpp
     CorrelationComputer.cpp
     loaders/LoadHDF5Data.cpp


)

set( SIMIL_LINK_LIBRARIES
     vmmlib
     ${Boost_LIBRARIES}
     ${HDF5_LIBRARIES}
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
