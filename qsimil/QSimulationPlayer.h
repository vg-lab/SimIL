/*
 * ClickableSlider.h
 *
 *  Created on: 11 de dic. de 2015
 *  Updated on: 15 de sept. de 2016
 *      Author: sgalindo
 *      Author: crodriguez
 */

#ifndef __QSIMIL__QSIMULATION_PLAYER_H__
#define __QSIMIL__QSIMULATION_PLAYER_H__

#include <QWidget>
#include <QDockWidget>
#include <QSlider>
#include <QPushButton>
#include <QLabel>
#include <simil/simil.h>
#include "ClickableSlider.h"

// TODO: Add zeroeq

namespace qsimil
{
    class QSimulationPlayer: public QWidget
    {
        Q_OBJECT
    public:
        QSimulationPlayer( QWidget *parent = 0 );

        void updateSlider( float percentage );

        bool isPlaying( void ) const;
        float getPercentage( void ) const;
        simil::SimulationPlayer* getSimulationPlayer( void ) const;
        void play( void );

        void init( const char* blueConfig, 
            simil::TSimulationType type, bool autoStart = false );
        void reset( void );
        void update( bool sendGIDS = false );
        void UpdateSimulationSlider( float percentage );

    protected:
        std::vector< uint32_t > _gidsSimulation;

        simil::SimulationPlayer *_simPlayer = nullptr;
        
        QDockWidget* _simulationDock;
        QSlider* _simSlider;
        QPushButton* _playButton;
        QLabel* _startTimeLabel;
        QLabel* _endTimeLabel;
        QPushButton* _repeatButton;

        QIcon _playIcon;
        QIcon _pauseIcon;

        bool _playing;
        float _percentage;

#ifdef TEVIMOS_USE_ZEROEQ
#ifdef TEVIMOS_USE_GMRVLEX
    void applyPlaybackOpertion( unsigned int playbackOp );
    void _zeqEventRepeat( bool repeat );
#endif
#endif

    protected slots:
        void _PlayPause( bool notify = true );
        void _Play( bool notify = true );
        void _Pause( bool notify = true );
        void _Stop( bool notify = true );
        void _Repeat( bool notify = true );
        void _PlayAt( bool notify = true );
        void _PlayAt( float, bool notify = true );
        void _PlayAt( int, bool notify = true );
        void _Restart( bool notify = true );
        void _GoToEnd( bool notify = true );
    
    };  // CustomPlayer
}; // qttevimos

#endif // __QSIMIL__QSIMULATION_PLAYER_H__
