/*
 * CustomSlider.h
 *
 *  Created on: 02 of nov., 2016
 *      Author: crodriguez
 */

#ifndef __QSIMIL__QSIMULATION_PLAYER_H__
#define __QSIMIL__QSIMULATION_PLAYER_H__

#include <QWidget>
#include <QDockWidget>
#include <QSlider>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QElapsedTimer>
#include <iostream>
#include <QComboBox>

#include <simil/simil.h>
#include <boost/signals2.hpp>

#include "ICustomPlayer.h"
#include "QCustomPlayer.h"

namespace qsimil
{
    class QSimulationPlayer: public QWidget
    {
        Q_OBJECT
    public:
        QSimulationPlayer( QWidget *parent = 0 );

        void init( ICustomPlayer* icp, const char* blueConfig, 
            bool autoStart = false );
        inline float& getStepsPerSecond() { return this->_stepsPerSecond; }
        inline float& getStepDeltaTime() { return this->_stepDeltaTime; }
        void setStepsPerSecond(const float& d);
        void setStepDeltaTime(const float& ws);
        inline bool& isPlaying() { return this->_q->isPlaying( ); }
        void update( void );
        void reset( void );
        simil::SpikesPlayer * getSimulation( void );

    public slots:
        void onStepsPerSecondChanged( QString );
        void handleStepDeltaTimeUpdate( );

    protected:
        float _stepsPerSecond;
        float _stepDeltaTime;
        QLineEdit* _deltaTime;
        QElapsedTimer counterNewFrame;

        QComboBox* stepPerSecond;

        QCustomPlayer* _q;

    };  // QSimulationPlayer
}; // qsimil

#endif // __QSIMIL__QSIMULATION_PLAYER_H__
