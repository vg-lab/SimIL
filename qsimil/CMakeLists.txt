
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

set( QSIMIL_PUBLIC_HEADERS
     CustomSlider.h
     ICustomPlayer.h
     QSimulationPlayer.h
     QCustomPlayer.h
)

set( QSIMIL_HEADERS
)

set( QSIMIL_SOURCES
     QCustomPlayer.cpp
     QSimulationPlayer.cpp
     resources_cp.qrc
)

set( QSIMIL_LINK_LIBRARIES
     SimIL
     Qt5::Core
     Qt5::Widgets
)

if( ZEROEQ_FOUND )
  list( APPEND QSIMIL_LINK_LIBRARIES ZeroEQ ${CMAKE_THREAD_LIBS_INIT} )
endif( )

if( GMRVLEX_FOUND )
  list( APPEND QSIMIL_LINK_LIBRARIES gmrvlex )
endif( )

if( LEXIS_FOUND )
  list( APPEND QSIMIL_LINK_LIBRARIES Lexis )
endif( )

add_definitions( -DQSIMIL_SHARED )

set( QSIMIL_INCLUDE_NAME qsimil )
set( QSIMIL_NAMESPACE qsimil )
common_library( QSimIL )

