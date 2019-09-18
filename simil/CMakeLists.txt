
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
     Spikes.hpp
     DataSet.h
     storage/Storage.h
     storage/StorageSparse.h
     storage/StorageInt.h
     storage/StorageFloat.h
     loaders/LoadSimData.h
     loaders/LoadblueConfigData.h
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
     H5SubsetEvents.cpp     CorrelationComputer.cpp
     DataSet.cpp
     storage/StorageSparse.cpp
     storage/StorageInt.cpp
     storage/StorageFloat.cpp
     loaders/LoadblueConfigData.cpp
     loaders/LoadHDF5Data.cpp
)

set( SIMIL_LINK_LIBRARIES
     vmmlib
     ${Boost_LIBRARIES}
     ${HDF5_LIBRARIES}
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
