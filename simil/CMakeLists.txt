
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
endif( )


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
