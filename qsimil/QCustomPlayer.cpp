/*
 * CustomSlider.cpp
 *
 *  Created on: 11 de dic. de 2015
 *  Updated on: 15 de sept. de 2016
 *      Author: sgalindo
 *      Author: crodriguez
 */
 
#include "QCustomPlayer.h"
#include <QGridLayout>

namespace qsimil
{
	QCustomPlayer::QCustomPlayer( QWidget *parent )
		: QWidget(parent)
	{
		{
			_simulationDock = new QDockWidget( );
			_simulationDock->setMinimumHeight( 100 );
			_simulationDock->setSizePolicy( QSizePolicy::MinimumExpanding,
											QSizePolicy::MinimumExpanding );
			unsigned int totalHSpan = 20;

			QGridLayout* dockLayout = new QGridLayout( this );

			_simSlider = new CustomSlider( Qt::Horizontal );
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
			//		2, 1, 1, Qt::AlignRight );

			row++;
			dockLayout->addWidget( _repeatButton, row, 6, 1, 1 );
			dockLayout->addWidget( prevButton, row, 7, 1, 1 );
			dockLayout->addWidget( _playButton, row, 8, 2, 2 );
			dockLayout->addWidget( nextButton, row, 10, 1, 1 );
			dockLayout->addWidget( stopButton, row, 11, 1, 1 );

			_playing = false;

			connect( _playButton, SIGNAL( clicked( )),
					 this, SLOT( _PlayPause( )));

			connect( stopButton, SIGNAL( clicked( )),
					   this, SLOT( _Stop( )));

			connect( nextButton, SIGNAL( clicked( )),
					   this, SLOT( _GoToEnd( )));

			connect( prevButton, SIGNAL( clicked( )),
					   this, SLOT( _Restart( )));

			connect( _repeatButton, SIGNAL( clicked( )),
					   this, SLOT( _Repeat( )));

			connect( _simSlider, SIGNAL( sliderPressed( )),
					 this, SLOT( _PlayAt( )));


  			// TODO: connect( _simSlider, SIGNAL( updateSlider( float )),
            // 		 this, SLOT( UpdateSimulationSlider( float )));

			_percentage = 0.0f;
		}
	}

	void QCustomPlayer::_PlayPause( bool notify )
	{
		if ( _playing ) {
			_Pause( notify );
		} else {
			_Play( notify );
		}
	}

	void QCustomPlayer::_Play( bool )
	{
		if ( _simPlayer )
		{
			_simPlayer->Play( );
			_playButton->setIcon( _pauseIcon );
			_playing = true;
			this->_icp->cpPlay();

			updateSlider( 0.0f );
		}
	}

	void QCustomPlayer::_Pause( bool )
	{
		if ( _simPlayer )
		{
			_simPlayer->Pause( );
			_playButton->setIcon( _playIcon );
			_playing = false;
			this->_icp->cpPause();
		}
	}

	void QCustomPlayer::_Stop( bool )
	{
		if ( _simPlayer )
		{
			_simPlayer->Stop( );
			_playButton->setIcon( _playIcon );
			_startTimeLabel->setText( QString::number( 
				(double)_simPlayer->startTime( ) ) );
			_playing = false;
			this->_icp->cpStop( );
		}
	}

	void QCustomPlayer::_Repeat( bool )
	{
		if( _simPlayer )
		{
			bool repeat = _repeatButton->isChecked( );
			_simPlayer->loop( repeat );
			this->_icp->cpClickRestart( repeat );
		}
	}

	void QCustomPlayer::_PlayAt( bool notify )
	{
		if( _simPlayer )
		{
			_PlayAt( _simSlider->sliderPosition( ), notify );
		}
	}

	void QCustomPlayer::_PlayAt( float percentage, bool notify )
	{
		if( _simPlayer )
		{
			int sliderPos = percentage *
				( _simSlider->maximum( ) - _simSlider->minimum( )) +
			_simSlider->minimum( );

			_PlayAt( sliderPos, notify );
		}
	}

	void QCustomPlayer::_PlayAt( int sliderPosition, bool )
	{
		if( _simPlayer )
		{
			int value = _simSlider->value( );
			this->_percentage = float( value - _simSlider->minimum( )) /
							   float( _simSlider->maximum( ) - 
							   	_simSlider->minimum( ));
			_simSlider->setSliderPosition( sliderPosition );

			_playButton->setIcon( _pauseIcon );

			_simPlayer->PlayAt( this->_percentage );
			_playing = true;
			this->_icp->cpOnPlayAt( this->_percentage );
		}
	}

	void QCustomPlayer::_Restart( bool )
	{
		if( _simPlayer )
		{
			updateSlider( 0.0f );
			bool playing = _playing;
			_simPlayer->Stop( );
			_playing = false;
			_simSlider->setSliderPosition(_simSlider->minimum( ));
			this->_icp->cpGoInit( );
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
		}
	}

	void QCustomPlayer::_GoToEnd( bool )
	{
		if( _simPlayer )
		{
			// TODO
			_simSlider->setSliderPosition(_simSlider->maximum( ));
			this->_icp->cpGoEnd();
			updateSlider( 1.0f );
		}
	}

	void QCustomPlayer::updateSlider( float percentage )
	{
		this->_percentage = percentage;
		int sliderPosition = percentage *
				( _simSlider->maximum( ) - _simSlider->minimum( )) +
				_simSlider->minimum( );
		_simSlider->setSliderPosition( sliderPosition );
		_startTimeLabel->setText( QString::number( std::floor(
			percentage * 100.0f * 100.) / 100. ) + QString( "%") );
	}

#ifdef TEVIMOS_USE_ZEROEQ
#ifdef TEVIMOS_USE_GMRVLEX
	void QCustomPlayer::applyPlaybackOpertion( unsigned int playbackOp )
	{
		//zeroeq::gmrv::PlaybackOperation operation =
		//    ( zeroeq::gmrv::PlaybackOperation ) playbackOp;

		auto operation = playbackOp;

		switch( operation )
		{
		  case 0: // TODO: case zeroeq::gmrv::PLAY:
				std::cout << "Received play" << std::endl;
				_Play( false );
				break;
		  case 1: // TODO: case zeroeq::gmrv::PAUSE:
				_Pause( false );
				std::cout << "Received pause" << std::endl;
				break;
		  case 2: // TODO: case zeroeq::gmrv::STOP:
				std::cout << "Received stop" << std::endl;
				_Stop( false );
				break;
		  case 3: // TODO: case zeroeq::gmrv::BEGIN:
				std::cout << "Received begin" << std::endl;
				_Restart( false );
				break;
		  case 4: // TODO: case zeroeq::gmrv::END:
				std::cout << "Received end" << std::endl;
				_GoToEnd( false );
				break;
		  case 5: // TODO: case zeroeq::gmrv::ENABLE_LOOP:
				std::cout << "Received enable loop" << std::endl;
				_zeqEventRepeat( true );
				break;
		  case 6: // TODO: case zeroeq::gmrv::DISABLE_LOOP:
				std::cout << "Received disable loop" << std::endl;
				_zeqEventRepeat( false );
				break;
		  default:
			break;
		}
	}

  	void QCustomPlayer::_zeqEventRepeat( bool repeat )
	{
		_repeatButton->setChecked( repeat );
	}

#endif
#endif
	

    void QCustomPlayer::init( ICustomPlayer* icp, 
    	const char* blueConfig, bool autoStart )
    {
        setICustomPlayer( icp );

        simil::TDataType dataType( simil::TDataType::TBlueConfig );
        std::string path = blueConfig; 

        _simPlayer = new simil::SpikesPlayer();
        _simPlayer->LoadData( dataType, path );

        // std::cout << "SpikesPlayer loaded ..." << std::endl;

        updateSlider( 0.0f );

        _startTimeLabel->setText(
            QString( "0.0%" ) );

        _endTimeLabel->setText(
            QString::number( (double)_simPlayer->endTime( )));

        if ( autoStart ) 
        {
            this->_Play( );
        }
    }

    void QCustomPlayer::reset( void )
    {
        this->_simPlayer->Reset( );
        this->_Restart( );
    }

    void QCustomPlayer::update( bool sendGIDS )
    {
        int value = _simSlider->value( );
        // std::cout << "UPDATE =" << value << std::endl;
        _simPlayer->Frame();
        UpdateSimulationSlider ( this->_simPlayer->GetRelativeTime( ) );

        if ( sendGIDS )
        {
            _simPlayer->spikesNowVect( _gidsSimulation );
            this->_icp->cpChangeGIDSimulation( _gidsSimulation );
        }
    }

    void QCustomPlayer::UpdateSimulationSlider( float percentage )
    {
        _startTimeLabel->setText(
            QString::number( (double)this->_simPlayer->currentTime( )) 
            + QString("%"));

        int total = _simSlider->maximum( ) - _simSlider->minimum( );

        int position = percentage * total;

        _simSlider->setSliderPosition( position );
        // std::cout << "UpdateSimulationSlider = " << percentage << 
		// 		std::endl;
    }
};
