set(CMAKE_CXX_STANDARD 14)
set( EXAMPLES_LINK_LIBRARIES)

set( SIMILREFACTORLOADEXAMPLE_SOURCES loadRefactorExample.cpp )
set( SIMILREFACTORLOADEXAMPLE_HEADERS )
set( SIMILREFACTORLOADEXAMPLE_LINK_LIBRARIES ${EXAMPLESH5_LINK_LIBRARIES} SimIL )
common_application( similRefactorLoadExample )

set( SIMILLOADEXAMPLE_SOURCES loadExample.cpp )
set( SIMILLOADEXAMPLE_HEADERS )
set( SIMILLOADEXAMPLE_LINK_LIBRARIES ${EXAMPLESH5_LINK_LIBRARIES} SimIL )
common_application( similLoadExample )

set( SIMILSUBSET_SOURCES subset.cpp )
set( SIMILSUBSET_HEADERS )
set( SIMILSUBSET_LINK_LIBRARIES ${EXAMPLESH5_LINK_LIBRARIES} SimIL )
common_application( similSubset )

if (SIMIL_USE_CONE)
    set( SPIKESENDER_SOURCES spike_sender.cpp )
    set( SPIKESENDER_HEADERS )
    set( SPIKESENDER_LINK_LIBRARIES SimIL cone::cone contra::contra nesci::producer )
    common_application (spikesender)


    set( DUMMYRECEIVER_SOURCES dummy_receiver.cpp )
    set( DUMMYRECEIVER_HEADERS )
    set( DUMMYRECEIVER_LINK_LIBRARIES cone::cone  )
    common_application (dummyreceiver)
endif()
