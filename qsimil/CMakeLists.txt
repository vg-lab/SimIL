
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

set( QSIMIL_PUBLIC_HEADERS
     CustomPlayer.h
     CustomSlider.h
)

set( QSIMIL_HEADERS
)

set( QSIMIL_SOURCES
     CustomPlayer.cpp
     resources_cp.qrc
)

set( QSIMIL_LINK_LIBRARIES
     Qt5::Core
     Qt5::Widgets
)

add_definitions( -DQSIMIL_SHARED )

set( QSIMIL_INCLUDE_NAME qsimil )
set( QSIMIL_NAMESPACE qsimil )
common_library( QSimIL )

