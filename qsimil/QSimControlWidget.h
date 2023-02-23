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

#ifndef __QSIMIL__QSIMCONTROL_WIDGET_H__
#define __QSIMIL__QSIMCONTROL_WIDGET_H__

#include <QWidget>
#include <QDockWidget>
#include <QSlider>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QElapsedTimer>
#include <QDoubleSpinBox>
#include <QTimer>

#include <iostream>

#include <simil/simil.h>
#include <boost/signals2.hpp>

#include "QSimulationPlayer.h"
#include <qsimil/api.h>


namespace qsimil
{
  class QSIMIL_API QSimControlWidget: public QWidget
  {
      Q_OBJECT
  public:
    QSimControlWidget( QWidget *parent = 0 );

    void init( const char* blueConfig, simil::TSimulationType type, 
      bool autoStart = false );

    void init(simil::SimulationPlayer* player, bool autoStart = false);

    float getStepsPerSecond() const;
    float getStepDeltaTime() const;
    bool isPlaying() const;

    void setStepsPerSecond(const float& d);
    void setStepDeltaTime(const float& dt);
    void reset();
    simil::SimulationPlayer * getSimulationPlayer();

  signals:
    void frame();

  public slots:
    void onStepsPerSecondChanged(double);
    void onDeltaTimeChanged(double);
    void update();

  protected:
    void updateValues( );

    float _stepsPerSecond;
    float _stepDeltaTime;
    QTimer m_timer;

    QSimulationPlayer* _qSimPlayer;

  };  // QSimControlWidget
}; // qsimil

#endif // __QSIMIL__QSIMCONTROL_WIDGET_H__
