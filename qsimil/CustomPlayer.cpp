/*
 * CustomSlider.cpp
 *
 *  Created on: 11 de dic. de 2015
 *  Updated on: 15 de sept. de 2016
 *      Author: sgalindo
 *      Author: crodriguez
 */
 
#include "CustomPlayer.h"
#include "CustomSlider.h"
#include <QGridLayout>

namespace qsimil
{
    CustomPlayer::CustomPlayer( QWidget *parent )
        : QWidget(parent)
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

        _playIcon.addFile( QStringLiteral( ":/icons/play.png" ), QSize( ),
                          QIcon::Normal, QIcon::Off );
        _pauseIcon.addFile( QStringLiteral( ":/icons/pause.png" ), QSize( ),
                           QIcon::Normal, QIcon::Off) ;
        stopIcon.addFile( QStringLiteral( ":/icons/stop.png" ), QSize( ),
                          QIcon::Normal, QIcon::Off );
        nextIcon.addFile( QStringLiteral( ":/icons/next.png" ), QSize( ),
                          QIcon::Normal, QIcon::Off );
        prevIcon.addFile( QStringLiteral( ":/icons/previous.png" ), QSize( ),
                          QIcon::Normal, QIcon::Off );
        repeatIcon.addFile( QStringLiteral( ":/icons/repeat.png" ), QSize( ),
                            QIcon::Normal, QIcon::Off );

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
        dockLayout->addWidget( _simSlider, row, 1, 1, totalHSpan - 3 );
        dockLayout->addWidget( _endTimeLabel, row, totalHSpan - 2, 1, 1, Qt::AlignRight );

        row++;
        dockLayout->addWidget( _repeatButton, row, 6, 1, 1 );
        dockLayout->addWidget( prevButton, row, 7, 1, 1 );
        dockLayout->addWidget( _playButton, row, 8, 2, 2 );
        dockLayout->addWidget( nextButton, row, 10, 1, 1 );
        dockLayout->addWidget( stopButton, row, 11, 1, 1 );

        connect( _playButton, SIGNAL( clicked( )),
                 this, SLOT( PlayPause( )));

        connect( stopButton, SIGNAL( clicked( )),
                   this, SLOT( Stop( )));

        connect( nextButton, SIGNAL( clicked( )),
                   this, SLOT( GoToEnd( )));

        connect( prevButton, SIGNAL( clicked( )),
                   this, SLOT( Restart( )));

        connect( _repeatButton, SIGNAL( clicked( )),
                   this, SLOT( Repeat( )));

        connect( _simSlider, SIGNAL( sliderPressed( )),
                 this, SLOT( PlayAt( )));
    }
    void CustomPlayer::start() {
        this->_playButton->click();
    }

    void CustomPlayer::PlayPause( bool notify )
    {
        if ( playing == true ) {
            Play( notify );
        } else {
            Pause( notify );
        }
        playing = !playing;
    }

    void CustomPlayer::Play( bool )
    {
        _playButton->setIcon( _pauseIcon );

        onPlay();
    }

    void CustomPlayer::Pause( bool )
    {
        _playButton->setIcon( _playIcon );

        onPause();
    }

    void CustomPlayer::Stop( bool )
    {
        _playButton->setIcon( _playIcon );
        _startTimeLabel->setText( QString::number( 0.0 ) ); //(double)_openGLWidget->player( )->startTime( )));

        onStop();
    }

    void CustomPlayer::Repeat( bool )
    {
        bool repeat = _repeatButton->isChecked( );
        onRepeat( repeat );
    }

    void CustomPlayer::PlayAt( bool notify )
    {
        PlayAt(_simSlider->sliderPosition(), notify);
    }

    void CustomPlayer::PlayAt( float percentage, bool )
    {
        int sliderPosition = percentage *
                ( _simSlider->maximum( ) - _simSlider->minimum( )) +
                _simSlider->minimum( );
        _simSlider->setSliderPosition( sliderPosition );
        _playButton->setIcon( _pauseIcon );

        onPlayAt( percentage );
    }

    void CustomPlayer::PlayAt( int sliderPosition, bool )
    {
        playing = true;
        int value = _simSlider->value( );
        float percentage = float( value - _simSlider->minimum( )) /
                           float( _simSlider->maximum( ) - _simSlider->minimum( ));
        _simSlider->setSliderPosition( sliderPosition );
        _playButton->setIcon( _pauseIcon );

        onPlayAt( percentage );
    }

    void CustomPlayer::Restart( bool )
    {
        _playButton->setIcon( _pauseIcon );
        if (playing) {
            _playButton->setIcon( _pauseIcon );
        } else {
            _playButton->setIcon( _playIcon );
        }
        _simSlider->setSliderPosition(_simSlider->minimum( ));

        onRestart();
    }

    void CustomPlayer::GoToEnd( bool )
    {
        _simSlider->setSliderPosition(_simSlider->maximum( ));

        onGoToEnd();
    }
};