/*
 * CustomSlider.h
 *
 *  Created on: 11 de dic. de 2015
 *  Updated on: 15 de sept. de 2016
 *      Author: sgalindo
 *      Author: crodriguez
 */

#ifndef __QSIMIL__QCUSTOMPLAYER_H__
#define __QSIMIL__QCUSTOMPLAYER_H__

#include <QWidget>
#include <QDockWidget>
#include <QSlider>
#include <QPushButton>
#include <QLabel>
#include <simil/simil.h>
#include "ICustomPlayer.h"
#include "CustomSlider.h"

// TODO: Add zeroeq

namespace qsimil
{
    class QCustomPlayer: public QWidget
    {
        Q_OBJECT
    public:
        QCustomPlayer( QWidget *parent = 0 );

        void updateSlider( float percentage );

        bool& isPlaying() { return this->_playing; }
        float& getPercentage() { return this->_percentage; }

        inline void setICustomPlayer( ICustomPlayer* icp )
        {
            this->_icp = icp;
        }

        inline ICustomPlayer* getICustomPlayer( )
        {
            return this->_icp;
        }
        void init( ICustomPlayer* icp, const char* blueConfig, 
            bool autoStart = false );
        void reset( void );
        void update( bool sendGIDS = false );
        void UpdateSimulationSlider( float percentage );
        inline simil::SpikesPlayer* getSpikesPlayer( )
        {
            return this->_simPlayer;
        }

    protected:
        std::vector< uint32_t > _gidsSimulation;

        ICustomPlayer* _icp = nullptr;
        simil::SpikesPlayer *_simPlayer = nullptr;
        
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

#endif // __QSIMIL__QCUSTOMPLAYER_H__
