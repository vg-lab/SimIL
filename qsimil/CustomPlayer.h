/*
 * CustomSlider.h
 *
 *  Created on: 11 de dic. de 2015
 *  Updated on: 15 de sept. de 2016
 *      Author: sgalindo
 *      Author: crodriguez
 */

#ifndef __SIMIL__CUSTOMPLAYER_H__
#define __SIMIL__CUSTOMPLAYER_H__

#include <QWidget>
#include <QDockWidget>
#include <QSlider>
#include <QPushButton>
#include <QLabel>

namespace qsimil
{
    class CustomPlayer: public QWidget
    {
    public:
        CustomPlayer( QWidget *parent = 0 );

        bool playing = true;
        /**
         * @brief onPlay
         * Called on play event.
         */
        virtual void onPlay() = 0;
        /**
         * @brief onPause
         * Called on pause event.
         */
        virtual void onPause() = 0;
        /**
         * @brief onRestart
         * Called on restart event.
         */
        virtual void onRestart() = 0;
        /**
         * @brief onGoToEnd
         * Called on go end event.
         */
        virtual void onGoToEnd() = 0;
        /**
         * @brief onRepeat
         * @param status (pressed status)
         * Called on repeat click event.
         */
        virtual void onRepeat( bool status ) = 0;
        /**
         * @brief onStop
         * Called on stop event.
         */
        virtual void onStop() = 0;
        /**
         * @brief onPlayAt
         * @param percentage
         * Called on play at event (when move slider).
         */
        virtual void onPlayAt( float percentage ) = 0;

        /**
         * @brief start
         * Start player.
         */
        void start();

        // TODO: void setStarTimeLabel( Qstring str );

    protected:
        QDockWidget* _simulationDock;
        QSlider* _simSlider;
        QPushButton* _playButton;
        QLabel* _startTimeLabel;
        QLabel* _endTimeLabel;
        QPushButton* _repeatButton;

        QIcon _playIcon;
        QIcon _pauseIcon;

    private slots:
        void PlayPause( bool notify = true );
        void Play( bool notify = true );
        void Pause( bool notify = true );
        void Stop( bool notify = true );
        void Repeat( bool notify = true );
        void PlayAt( bool notify = true );
        void PlayAt( float, bool notify = true );
        void PlayAt( int, bool notify = true );
        void Restart( bool notify = true );
        void GoToEnd( bool notify = true );
    };  // CustomPlayer
}; // qsimil

#endif // __SIMIL__CUSTOMPLAYER_H__
