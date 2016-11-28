/*
 * ClickableSlider.cpp
 *
 *  Created on: 11 de dic. de 2015
 *  Updated on: 15 de sept. de 2016
 *      Author: sgalindo
 *      Author: crodriguez
 */
 
#include "QSimulationPlayer.h"
#include <QGridLayout>

namespace qsimil
{
  QSimulationPlayer::QSimulationPlayer( QWidget *parent )
    : QWidget(parent)
  {
    {
      _simulationDock = new QDockWidget( );
      _simulationDock->setMinimumHeight( 100 );
      _simulationDock->setSizePolicy( QSizePolicy::MinimumExpanding,
                      QSizePolicy::MinimumExpanding );
      unsigned int totalHSpan = 20;

      QGridLayout* dockLayout = new QGridLayout( this );

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
      // dockLayout->addWidget( _endTimeLabel, row, totalHSpan - 
      //      2, 1, 1, Qt::AlignRight );

      row++;
      dockLayout->addWidget( _repeatButton, row, 6, 1, 1 );
      dockLayout->addWidget( prevButton, row, 7, 1, 1 );
      dockLayout->addWidget( _playButton, row, 8, 2, 2 );
      dockLayout->addWidget( nextButton, row, 10, 1, 1 );
      dockLayout->addWidget( stopButton, row, 11, 1, 1 );

      _playing = false;

      connect( _playButton, SIGNAL( clicked( )),
           this, SLOT( _playPause( )));

      connect( stopButton, SIGNAL( clicked( )),
             this, SLOT( _Stop( )));

      connect( nextButton, SIGNAL( clicked( )),
             this, SLOT( _GoToEnd( )));

      connect( prevButton, SIGNAL( clicked( )),
             this, SLOT( _restart( )));

      connect( _repeatButton, SIGNAL( clicked( )),
             this, SLOT( _Repeat( )));

      connect( _simSlider, SIGNAL( sliderPressed( )),
           this, SLOT( _playAt( )));


      // TODO: connect( _simSlider, SIGNAL( updateSlider( float )),
      //       this, SLOT( updateSimulationSlider( float )));

      _percentage = 0.0f;
    }
  }
  QSimulationPlayer::~QSimulationPlayer( void )
  {
    delete _simPlayer;
  }

  void QSimulationPlayer::_playPause( bool notify )
  {
    if ( _playing ) {
      _pause( notify );
    } else {
      _play( notify );
    }
  }

  void QSimulationPlayer::_play( bool notify )
  {
    if ( _simPlayer )
    {
      _simPlayer->Play( );
      _playButton->setIcon( _pauseIcon );
      _playing = true;

      updateSlider( 0.0f );

      if( notify )
      {
    #ifdef SIMIL_USE_ZEROEQ
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
      if( notify )
      {
    #ifdef SIMIL_USE_ZEROEQ
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
      _startTimeLabel->setText( QString::number( 
        (double)_simPlayer->startTime( ) ) );
      _playing = false;

      if( notify )
      {
    #ifdef SIMIL_USE_ZEROEQ
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
      _playAt( _simSlider->sliderPosition( ), notify );
    }
  }

  void QSimulationPlayer::_playAt( float percentage, bool notify )
  {
    if( _simPlayer )
    {
      int sliderPos = percentage *
        ( _simSlider->maximum( ) - _simSlider->minimum( )) +
      _simSlider->minimum( );

      _playAt( sliderPos, notify );
    }
  }

  void QSimulationPlayer::_playAt( int sliderPosition, bool notify )
  {
    if( _simPlayer )
    {
      int value = _simSlider->value( );
      this->_percentage = float( value - _simSlider->minimum( ) ) /
                 float( _simSlider->maximum( ) - 
                _simSlider->minimum( ) );
      _simSlider->setSliderPosition( sliderPosition );

      _playButton->setIcon( _pauseIcon );

      _simPlayer->PlayAt( this->_percentage );
      _playing = true;
      
    if( notify )
      {
    #ifdef SIMIL_USE_ZEROEQ
      // Send event
      _simPlayer->zeqEvents( )->sendFrame( _simSlider->minimum( ),
                             _simSlider->maximum( ),
                             sliderPosition );

      _simPlayer->zeqEvents( )->sendPlaybackOp( zeroeq::gmrv::PLAY );
    #endif
      }
    }
  }

  void QSimulationPlayer::_restart( bool notify )
  {
    if( _simPlayer )
    {
      updateSlider( 0.0f );
      bool playing = _playing;
      _simPlayer->Stop( );
      _playing = false;
      _simSlider->setSliderPosition(_simSlider->minimum( ));
      // TODO: this->_icp->cpGoInit( );
      if( playing )
      {
        _simPlayer->Play( );
        _playing = true;
      }
      if( _playing )
      {
        _playButton->setIcon( _pauseIcon );
      }
      else
      {
        _playButton->setIcon( _playIcon );

      }

      if( notify )
      {
    #ifdef SIMIL_USE_ZEROEQ
        _simPlayer->zeqEvents( )->sendPlaybackOp( zeroeq::gmrv::BEGIN );
    #endif
      }
    }
  }

  void QSimulationPlayer::_goToEnd( bool notify )
  {
    if( _simPlayer )
    {
      // TODO
      _simSlider->setSliderPosition(_simSlider->maximum( ));
      // TODO: this->_icp->cpGoEnd();
      updateSlider( 1.0f );
      if( notify )
      {
    #ifdef SIMIL_USE_ZEROEQ
       _simPlayer->zeqEvents( )->sendPlaybackOp( zeroeq::gmrv::END );
    #endif
      }
    }
  }

  void QSimulationPlayer::updateSlider( float percentage )
  {
    this->_percentage = percentage;
    int sliderPosition = percentage *
        ( _simSlider->maximum( ) - _simSlider->minimum( )) +
        _simSlider->minimum( );
    _simSlider->setSliderPosition( sliderPosition );
    _startTimeLabel->setText( QString::number( std::floor(
      percentage * 100.0f * 100.f) / 100.f ) + QString( "%") );
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
        std::cout << "Received play" << std::endl;
        _play( false );
        break;
      case zeroeq::gmrv::PAUSE:
        _Pause( false );
        std::cout << "Received pause" << std::endl;
        break;
      case zeroeq::gmrv::STOP:
        std::cout << "Received stop" << std::endl;
        _Stop( false );
        break;
      case zeroeq::gmrv::BEGIN:
        std::cout << "Received begin" << std::endl;
        _restart( false );
        break;
      case zeroeq::gmrv::END:
        std::cout << "Received end" << std::endl;
        _GoToEnd( false );
        break;
      case zeroeq::gmrv::ENABLE_LOOP:
        std::cout << "Received enable loop" << std::endl;
        _zeroeqEventRepeat( true );
        break;
      case zeroeq::gmrv::DISABLE_LOOP:
        std::cout << "Received disable loop" << std::endl;
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
    simil::TSimulationType type,  bool autoStart )
  {
    simil::TDataType dataType( simil::TDataType::TBlueConfig );
    std::string path = std::string( blueConfig ); 

    switch (type) {
      case simil::TSimulationType::TSimNetwork:
        break;
      case simil::TSimulationType::TSimSpikes:
        _simPlayer = new simil::SpikesPlayer();
        break;
      case simil::TSimulationType::TSimVoltages:
        _simPlayer = new simil::VoltagesPlayer();
        break;
    }
    _simPlayer->LoadData( dataType, path );

    // std::cout << "SimulationPlayer loaded ..." << std::endl;

    updateSlider( 0.0f );

    _startTimeLabel->setText( QString( "0.0%" ) );

    //_endTimeLabel->setText(
    //  QString::number( (double)_simPlayer->endTime( )));

    if ( autoStart ) 
    {
      this->_play( );
    }
  }

  void QSimulationPlayer::reset( void )
  {
    this->_simPlayer->Reset( );
    this->_restart( );
  }

  void QSimulationPlayer::update( bool sendGIDS )
  {
    //std::cout << "PIDO FRAME" << std::endl;
    _simPlayer->Frame();
    //std::cout << "UPDATE SLIDER" << std::endl;
    updateSimulationSlider ( this->_simPlayer->GetRelativeTime( ) );

    if ( sendGIDS )
    {
      // TODO
      ((simil::SpikesPlayer*)_simPlayer)->spikesNowVect( _gidsSimulation );
      // TODO: this->_icp->cpChangeGIDSimulation( _gidsSimulation );
    }
  }

  void QSimulationPlayer::updateSimulationSlider( float percentage )
  {
    _startTimeLabel->setText(
      QString::number( (double)this->_simPlayer->currentTime( )) 
      + QString("%"));

    int total = _simSlider->maximum( ) - _simSlider->minimum( );

    int position = percentage * total;

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
};
