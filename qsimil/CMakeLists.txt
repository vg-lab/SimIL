
set( QSIMIL_PUBLIC_HEADERS
  ClickableSlider.h
  QSimControlWidget.h
  QSimulationPlayer.h
)

set( QSIMIL_HEADERS
)

set( QSIMIL_SOURCES
  QSimulationPlayer.cpp
  QSimControlWidget.cpp
  resources_cp.qrc
)

set( QSIMIL_LINK_LIBRARIES
  SimIL
  ${Boost_LIBRARIES}
  ${Boost_SYSTEM_LIBRARY}
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

