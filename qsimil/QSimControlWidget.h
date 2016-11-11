/*
 * ClickableSlider.h
 *
 *  Created on: 02 of nov., 2016
 *      Author: crodriguez
 */

#ifndef __QSIMIL__QSIMCONTROL_WIDGET_H__
#define __QSIMIL__QSIMCONTROL_WIDGET_H__

#include <QWidget>
#include <QDockWidget>
#include <QSlider>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QElapsedTimer>
#include <iostream>
#include <QDoubleSpinBox>

#include <simil/simil.h>
#include <boost/signals2.hpp>

#include "QSimulationPlayer.h"

namespace qsimil
{
    class QSimControlWidget: public QWidget
    {
        Q_OBJECT
    public:
        QSimControlWidget( QWidget *parent = 0 );

        void init( const char* blueConfig, simil::TSimulationType type, 
            bool autoStart = false );

        inline float getStepsPerSecond( void ) const;
        inline float getStepDeltaTime( void ) const;
        inline bool isPlaying( void ) const;

        void setStepsPerSecond(const float& d);
        void setStepDeltaTime(const float& ws);
        void update( void );
        void reset( void );
        simil::SimulationPlayer * getSimulationPlayer( void );

    public slots:
        void onStepsPerSecondChanged( QString );
        void handleStepDeltaTimeUpdate( );

    protected:
        float _stepsPerSecond;
        float _stepDeltaTime;
        QLineEdit* _deltaTime;
        QElapsedTimer counterNewFrame;

        QDoubleSpinBox *stepPerSecond;

        QSimulationPlayer* _qSimPlayer;

    };  // QSimControlWidget
}; // qsimil

#endif // __QSIMIL__QSIMCONTROL_WIDGET_H__
