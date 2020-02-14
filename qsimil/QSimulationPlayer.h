/*
 * Copyright (c) 2015-2020 GMRV/URJC.
 *
 * Authors: Sergio E. Galindo <sergio.galindo@urjc.es>
 *          Cristian Rodriguez <cristian.rodriguez@urjc.es>
 *
 * This file is part of SimIL <https://github.com/gmrvvis/SimIL>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
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
    ~QSimulationPlayer( void );

    void updateSlider( float percentage );

    bool isPlaying( void ) const;
    float getPercentage( void ) const;
    simil::SimulationPlayer* getSimulationPlayer( void ) const;
    void play( void );

    void init( const char* blueConfig, 
      simil::TSimulationType type, bool autoStart = false );
    void reset( void );
    void update( bool sendGIDS = false );
    void updateSimulationSlider( float percentage );

  protected:
    std::vector< uint32_t > _gidsSimulation;

    simil::SimulationPlayer *_simPlayer;
    
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

  protected slots:
    void _playPause( bool notify = true );
    void _play( bool notify = true );
    void _pause( bool notify = true );
    void _stop( bool notify = true );
    void _repeat( bool notify = true );
    void _playAt( bool notify = true );
    void _playAt( float, bool notify = true );
    void _playAt( int, bool notify = true );
    void _restart( bool notify = true );
    void _goToEnd( bool notify = true );
  
  };  // QSimulationPlayer

}; // qsimil

#endif // __QSIMIL__QSIMULATION_PLAYER_H__
