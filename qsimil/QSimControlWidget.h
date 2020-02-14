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

    float getStepsPerSecond( void ) const;
    float getStepDeltaTime( void ) const;
    bool isPlaying( void ) const;

    void setStepsPerSecond(const float& d);
    void setStepDeltaTime(const float& dt);
    void update( void );
    void reset( void );
    simil::SimulationPlayer * getSimulationPlayer( void );

  public slots:
    void onStepsPerSecondChanged( const QString& s_ );
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
