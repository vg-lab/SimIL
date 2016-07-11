set( SIMIL_PUBLIC_HEADERS
     types.h
     log.h
     SimulationPlayer.h
     H5Network.h
     H5Activity.h
     SimulationData.h

)

set( SIMIL_HEADERS
)

set( SIMIL_SOURCES
     SimulationPlayer.cpp
     H5Network.cpp
     H5Activity.cpp
     SimulationData.cpp
)

set( SIMIL_LINK_LIBRARIES
     Brion
     Brain
     ${EXAMPLESH5_LINK_LIBRARIES}      
)

if( ZEROEQ_FOUND )
  list( APPEND SIMIL_LINK_LIBRARIES ZeroEQ ${CMAKE_THREAD_LIBS_INIT} )
endif( )

if( GMRVLEX_FOUND )
  list( APPEND SIMIL_LINK_LIBRARIES gmrvlex )
endif( )

if( LEXIS_FOUND )
  list( APPEND SIMIL_LINK_LIBRARIES Lexis )
endif( )

set( SIMIL_INCLUDE_NAME simil )
set( SIMIL_NAMESPACE simil )
add_definitions( -DSIMIL_SHARED )
common_library( SimIL )


