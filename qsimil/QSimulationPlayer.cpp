/*
 * Copyright (c) 2015-2020 VG-Lab/URJC.
 *
 * Authors: Sergio E. Galindo <sergio.galindo@urjc.es>
 *          Cristian Rodriguez <cristian.rodriguez@urjc.es>
 *
 * This file is part of SimIL <https://github.com/vg-lab/SimIL>
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
 
#include "QSimulationPlayer.h"
#include <QGridLayout>

namespace qsimil
{
  QSimulationPlayer::QSimulationPlayer( QWidget *parent_ )
  : QWidget(parent_)
  , _simPlayer( nullptr )
  {
    {
      _simulationDock = new QDockWidget( );
      _simulationDock->setMinimumHeight( 100 );
      _simulationDock->setSizePolicy( QSizePolicy::MinimumExpanding,
                      QSizePolicy::MinimumExpanding );
      unsigned int totalHSpan = 20;

      auto dockLayout = new QGridLayout( this );

      _simSlider = new ClickableSlider( Qt::Horizontal );
      _simSlider->setMinimum( 0 );
      _simSlider->setMaximum( 1000 );
      _simSlider->setSizePolicy( QSizePolicy::Preferred,
                     QSizePolicy::Preferred );

      _playButton = new QPushButton( );
      _playButton->setSizePolicy( QSizePolicy::MinimumExpanding,
                     QSizePolicy::MinimumExpanding );
      QPushButton* stopButton = new QPushButton( );
      QPushButton* nextButton = new QPushButton( );
      QPushButton* prevButton = new QPushButton( );

      _repeatButton = new QPushButton( );
      _repeatButton->setCheckable( true );
      _repeatButton->setChecked( false );

      QIcon stopIcon;
      QIcon nextIcon;
      QIcon prevIcon;
      QIcon repeatIcon;

      _playIcon.addFile( QStringLiteral( ":/icons/play.png" ), 
        QSize( ), QIcon::Normal, QIcon::Off );
      _pauseIcon.addFile( QStringLiteral( ":/icons/pause.png" ), 
        QSize( ), QIcon::Normal, QIcon::Off) ;
      stopIcon.addFile( QStringLiteral( ":/icons/stop.png" ), 
        QSize( ), QIcon::Normal, QIcon::Off );
      nextIcon.addFile( QStringLiteral( ":/icons/next.png" ), 
        QSize( ), QIcon::Normal, QIcon::Off );
      prevIcon.addFile( QStringLiteral( ":/icons/previous.png" ), 
        QSize( ), QIcon::Normal, QIcon::Off );
      repeatIcon.addFile( QStringLiteral( ":/icons/repeat.png" ), 
        QSize( ), QIcon::Normal, QIcon::Off );

      _playButton->setIcon( _playIcon );
      stopButton->setIcon( stopIcon );
      nextButton->setIcon( nextIcon );
      prevButton->setIcon( prevIcon );
      _repeatButton->setIcon( repeatIcon );

      _startTimeLabel = new QLabel( "" );
      _startTimeLabel->setSizePolicy( QSizePolicy::MinimumExpanding,
                      QSizePolicy::Preferred );
      _endTimeLabel = new QLabel( "" );
      _endTimeLabel->setSizePolicy( QSizePolicy::Preferred,
                      QSizePolicy::Preferred );

      unsigned int row = 2;
      dockLayout->addWidget( _startTimeLabel, row, 0, 1, 2 );
      dockLayout->addWidget( _simSlider, row, 2, 1, totalHSpan - 3 );
      dockLayout->addWidget( _endTimeLabel, row, 2+totalHSpan-3 +1, 1, 2);

      row++;
      dockLayout->addWidget( _repeatButton, row, 7, 1, 1 );
      dockLayout->addWidget( prevButton, row, 8, 1, 1 );
      dockLayout->addWidget( _playButton, row, 9, 2, 2 );
      dockLayout->addWidget( nextButton, row, 11, 1, 1 );
      dockLayout->addWidget( stopButton, row, 12, 1, 1 );

      _playing = false;

      connect( _playButton, SIGNAL( clicked( )),
           this, SLOT( _playPause( )));

      connect( stopButton, SIGNAL( clicked( )),
             this, SLOT( _stop( )));

      connect( nextButton, SIGNAL( clicked( )),
             this, SLOT( _goToEnd( )));

      connect( prevButton, SIGNAL( clicked( )),
             this, SLOT( _restart( )));

      connect( _repeatButton, SIGNAL( clicked( )),
             this, SLOT( _repeat( )));

      connect( _simSlider, SIGNAL( sliderPressed( )),
           this, SLOT( _playAt( )));

      _percentage = 0.0f;
    }
  }

  QSimulationPlayer::~QSimulationPlayer( void )
  {
    if(_simPlayer) delete _simPlayer;
  }

  void QSimulationPlayer::_playPause( bool notify )
  {
    if ( _playing )
      _pause( notify );
    else
      _play( notify );
  }

  void QSimulationPlayer::_play( bool notify )
  {
    if ( _simPlayer )
    {
      _simPlayer->Play( );
      _playButton->setIcon( _pauseIcon );
      _playing = true;

      emit playing();

      if( notify )
      {
    #ifdef SIMIL_USE_ZEROEQ
        if ( _simPlayer->zeqEvents( ))
        _simPlayer->zeqEvents( )->sendPlaybackOp( zeroeq::gmrv::PLAY );
    #endif
      }
    }
  }

  void QSimulationPlayer::_pause( bool notify )
  {
    if ( _simPlayer )
    {
      _simPlayer->Pause( );
      _playButton->setIcon( _playIcon );
      _playing = false;

      emit stopped();

      if( notify )
      {
    #ifdef SIMIL_USE_ZEROEQ
        if ( _simPlayer->zeqEvents( ))
          _simPlayer->zeqEvents( )->sendPlaybackOp( zeroeq::gmrv::PAUSE );
    #endif
      }
    }
  }

  void QSimulationPlayer::_stop( bool notify )
  {
    if ( _simPlayer )
    {
      _simPlayer->Stop( );
      _playButton->setIcon( _playIcon );
      _playing = false;

      const auto number = QString::number(_simPlayer->currentTime(),'f',3);
      _startTimeLabel->setText( number);

      emit stopped();

      if( notify )
      {
    #ifdef SIMIL_USE_ZEROEQ
        if ( _simPlayer->zeqEvents( ))
          _simPlayer->zeqEvents( )->sendPlaybackOp( zeroeq::gmrv::STOP );
    #endif
      }
    }
  }

  void QSimulationPlayer::_repeat( bool notify )
  {
    if( _simPlayer )
    {
      bool repeat = _repeatButton->isChecked( );
      _simPlayer->loop( repeat );

      if( notify )
      {
    #ifdef SIMIL_USE_ZEROEQ
        if ( _simPlayer->zeqEvents( ))
          _simPlayer->zeqEvents( )->sendPlaybackOp( repeat ?
                                                    zeroeq::gmrv::ENABLE_LOOP :
                                                    zeroeq::gmrv::DISABLE_LOOP );
    #endif
      }
    }
  }

  void QSimulationPlayer::_playAt( bool notify )
  {
    if( _simPlayer )
    {
      _playAtPosition( _simSlider->sliderPosition( ), notify );
    }
  }

  void QSimulationPlayer::_playAtPercentage( float percentage, bool notify )
  {
    if( _simPlayer )
    {
      const auto tBegin = _simPlayer->startTime();
      const auto tEnd   = _simPlayer->endTime();
      const auto timeStamp = percentage * (tEnd-tBegin) + tBegin;

      _playAtTime(timeStamp, notify);
    }
  }

  void QSimulationPlayer::_playAtPosition( int sliderPosition, bool notify )
  {
    if( _simPlayer )
    {
      const auto sMin = _simSlider->minimum();
      const auto sMax = _simSlider->maximum();
      sliderPosition = std::min(sMax, std::min(sMin, sliderPosition));
      const auto percentage = static_cast<float>(sliderPosition - sMin) / (sMax-sMin);

      _playAtPercentage(percentage, notify);
    }
  }

  void QSimulationPlayer::_playAtTime(float timeStamp, bool notify)
  {
    if(_simPlayer)
    {
      const float tBegin = _simPlayer->startTime();
      const float tEnd   = _simPlayer->endTime();
      timeStamp = std::min(tEnd, std::max(tBegin, timeStamp));

      const auto percentage = (timeStamp - tBegin) / (tEnd - tBegin);
      updateSlider( percentage );

      _playButton->setIcon( _pauseIcon );

      _simPlayer->PlayAtTime(timeStamp);
      _playing = true;

      emit playing();

      if( notify )
      {
#ifdef SIMIL_USE_ZEROEQ
        if ( _simPlayer->zeqEvents( ))
        {
          // Send event
          _simPlayer ->zeqEvents( )->sendFrame( _simPlayer->startTime(),
                                                _simPlayer->endTime(),
                                                _simPlayer->currentTime() );

          _simPlayer->zeqEvents( )->sendPlaybackOp( zeroeq::gmrv::PLAY );
        }
#endif
      }
    }
  }

  void QSimulationPlayer::_restart( bool notify )
  {
    if( _simPlayer )
    {
      updateSlider( 0.0f );
      const bool currentlyPlaying = _playing;
      _simPlayer->Stop( );
      _playing = false;

      emit stopped();

      _simSlider->setSliderPosition(_simSlider->minimum( ));
      // TODO: this->_icp->cpGoInit( );
      if( currentlyPlaying )
      {
        _simPlayer->Play( );
        _playing = true;

        emit playing();
      }

      _playButton->setIcon((_playing ? _pauseIcon : _playIcon));

      if( notify )
      {
    #ifdef SIMIL_USE_ZEROEQ
        if ( _simPlayer->zeqEvents( ))
          _simPlayer->zeqEvents( )->sendPlaybackOp( zeroeq::gmrv::BEGIN );
    #endif
      }
    }
  }

  void QSimulationPlayer::_goToEnd( bool notify )
  {
    if( _simPlayer )
    {
      updateSlider( 1.0f );
      if( notify )
      {
    #ifdef SIMIL_USE_ZEROEQ
        if ( _simPlayer->zeqEvents( ))
          _simPlayer->zeqEvents( )->sendPlaybackOp( zeroeq::gmrv::END );
    #endif
      }
    }
  }

  void QSimulationPlayer::updateSlider( float percentage )
  {
    this->_percentage = percentage;

    const int sMax = _simSlider->maximum();
    const int sMin = _simSlider->minimum();
    const int sliderPosition = percentage * ( sMax - sMin) + sMin;
    _simSlider->setSliderPosition( sliderPosition );

    const auto tBegin = _simPlayer->startTime();
    const auto tEnd = _simPlayer->endTime();
    const auto current = percentage * (tEnd-tBegin) + tBegin;
    const auto number = QString::number(current, 'f',3);
    _startTimeLabel->setText( number );
  }

#ifdef SIMIL_USE_ZEROEQ
#ifdef QSIMIL_USE_GMRVLEX
  void QSimulationPlayer::applyPlaybackOpertion( unsigned int playbackOp )
  {
    //zeroeq::gmrv::PlaybackOperation operation =
    //    ( zeroeq::gmrv::PlaybackOperation ) playbackOp;

    auto operation = playbackOp;

    switch( operation )
    {
      case zeroeq::gmrv::PLAY:
        _play( false );
        break;
      case zeroeq::gmrv::PAUSE:
        _pause( false );
        break;
      case zeroeq::gmrv::STOP:
        _stop( false );
        break;
      case zeroeq::gmrv::BEGIN:
        _restart( false );
        break;
      case zeroeq::gmrv::END:
        _goToEnd( false );
        break;
      case zeroeq::gmrv::ENABLE_LOOP:
        _zeroeqEventRepeat( true );
        break;
      case zeroeq::gmrv::DISABLE_LOOP:
        _zeroeqEventRepeat( false );
        break;
      default:
      break;
    }
  }

  void QSimulationPlayer::_zeroeqEventRepeat( bool repeat )
  {
    _repeatButton->setChecked( repeat );
  }

#endif
#endif
  

  void QSimulationPlayer::init( const char* blueConfig, 
    simil::TSimulationType type, bool autoStart )
  {
    simil::TDataType dataType( simil::TDataType::TBlueConfig );
    std::string path = std::string( blueConfig ); 

    switch (type) {
      case simil::TSimulationType::TSimNetwork:
        break;
      case simil::TSimulationType::TSimSpikes:
        _simPlayer = new simil::SpikesPlayer();
        break;
      default:
        {
          const auto message = std::string("Error: Simulation type not supported.\n") +
                               __FILE__ + ":" + std::to_string(__LINE__) + "\n";
          throw std::runtime_error(message);
        }
        return;
        break;
    }
    _simPlayer->LoadData( dataType, path );

    updateSlider( 0.0f );

    _startTimeLabel->setText(QString( "0.0" ));

    if ( autoStart ) this->_play( );
  }

  void QSimulationPlayer::reset( void )
  {
    this->_simPlayer->Reset( );
    this->_restart( );
  }

  void QSimulationPlayer::update( bool sendGIDS )
  {
    _simPlayer->Frame();

    emit frame();

    updateSimulationSlider ( this->_simPlayer->GetRelativeTime( ) );

    if ( sendGIDS )
    {
      // TODO
      //dynamic_cast<simil::SpikesPlayer*>(_simPlayer)->spikesNowVect(_gidsSimulation);
    }
  }

  void QSimulationPlayer::updateSimulationSlider( float percentage )
  {
    updateSimulationSlider();
    const int total = _simSlider->maximum( ) - _simSlider->minimum( );
    const int position = percentage * total;

    _simSlider->setSliderPosition( position );
  }

  bool QSimulationPlayer::isPlaying( void ) const
  {
    return this->_playing;
  }

  float QSimulationPlayer::getPercentage( void ) const
  {
    return this->_percentage;
  }

  simil::SimulationPlayer* QSimulationPlayer::getSimulationPlayer( void ) const
  {
    return this->_simPlayer;
  }

  void QSimulationPlayer::play( void )
  {
    _play( );
  }

  void QSimulationPlayer::init(simil::SimulationPlayer *player, bool autoStart)
  {
    if(!player) return;

    if(_simPlayer) delete _simPlayer;

    _simPlayer = player;

    _startTimeLabel->setText(QString( "0.0" ));

    const auto endTime = QString::number( _simPlayer->endTime(), 'f',3);
    _endTimeLabel->setText(endTime);

    updateSlider( 0.0f );

    if ( autoStart ) this->_play( );
  }

  void QSimulationPlayer::updateSimulationSlider()
  {
    if(_simPlayer)
    {
      const auto number = QString::number( _simPlayer->currentTime( ), 'f',3);
      _startTimeLabel->setText(number);
    }
  }
};
