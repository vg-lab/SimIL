set( EXAMPLES_LINK_LIBRARIES)

set( SIMILREFACTORLOADEXAMPLE_SOURCES loadRefactorExample.cpp )
set( SIMILREFACTORLOADEXAMPLE_HEADERS )
set( SIMILREFACTORLOADEXAMPLE_LINK_LIBRARIES ${EXAMPLESH5_LINK_LIBRARIES} QSimIL )
common_application( similRefactorLoadExample )

set( SIMILCSVEXAMPLE_SOURCES csv.cpp )
set( SIMILCSVEXAMPLE_HEADERS )
set( SIMILCSVEXAMPLE_LINK_LIBRARIES ${EXAMPLESH5_LINK_LIBRARIES} QSimIL )
common_application( similCSVExample )

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

    set( INSITUEXAMPLE_SOURCES InsituExample.cpp )
    set( INSITUEXAMPLE_HEADERS )
    set( INSITUEXAMPLE_LINK_LIBRARIES SimIL cone::cone  )
    common_application (InsituExample)
endif()
