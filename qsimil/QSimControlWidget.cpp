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

#include "QSimControlWidget.h"
#include <QGroupBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QDoubleSpinBox>

namespace qsimil
{
  QSimControlWidget::QSimControlWidget( QWidget *parent_)
    : QWidget(parent_)
    , _qSimPlayer( new QSimulationPlayer( ))
  {
    auto grid = new QHBoxLayout( );
    this->setLayout(grid);

    auto groupBox = new QGroupBox( tr("Simulation parameters") );
    groupBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    grid->addWidget(groupBox, 0);

    auto hbox = new QVBoxLayout();
    groupBox->setLayout(hbox);

    auto hLayout = new QHBoxLayout();
    hLayout->addWidget(new QLabel( tr( "Steps/Second" )));
    auto stepPerSecond = new QDoubleSpinBox( );

    hLayout->addWidget( stepPerSecond);
    stepPerSecond->setValue(1.0);
    stepPerSecond->setSingleStep( 1 );

    connect( stepPerSecond, SIGNAL( valueChanged(double)),
             this, SLOT( onStepsPerSecondChanged(double)));

    hbox->addLayout(hLayout);
    
    hLayout = new QHBoxLayout();
    auto deltaTime = new QDoubleSpinBox();
    deltaTime->setValue(0.25);
    hLayout->addWidget(new QLabel( tr( "Step Delta Time" )));
    hLayout->addWidget( deltaTime);

    connect( deltaTime, SIGNAL( valueChanged(double)),
             this, SLOT( onDeltaTimeChanged(double)));

    hbox->addLayout(hLayout);

    auto renderFuncMode = new QGroupBox( );
    renderFuncMode->setSizePolicy(QSizePolicy::Preferred, 
      QSizePolicy::Fixed);

    grid->addWidget(renderFuncMode, 1);
    QVBoxLayout* vlayout = new QVBoxLayout;
    renderFuncMode->setLayout( vlayout );
    
    vlayout->addWidget( _qSimPlayer );

    connect(_qSimPlayer, SIGNAL(frame()), this, SIGNAL(frame()));

    m_timer.setInterval(1000);
    m_timer.setSingleShot(false);
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(update()));
    connect(_qSimPlayer, SIGNAL(playing()), &m_timer, SLOT(start()));
    connect(_qSimPlayer, SIGNAL(stopped()), &m_timer,  SLOT(stop()));
  }

  void QSimControlWidget::onStepsPerSecondChanged(double d)
  {
    _stepsPerSecond = d;
    updateValues();
  }

  void QSimControlWidget::updateValues( )
  {
    if(getSimulationPlayer())
    {
      getSimulationPlayer()->deltaTime( this->_stepsPerSecond * this->_stepDeltaTime );
    }
  }

  void QSimControlWidget::init( const char* blueConfig, 
    simil::TSimulationType type, bool autoStart )
  {
    this->_qSimPlayer->init( blueConfig, type, autoStart );

    _stepsPerSecond = 1.0;
    _stepDeltaTime = 0.25;
    updateValues();
  }

  void QSimControlWidget::setStepsPerSecond(const float& d)
  {
    this->_stepsPerSecond = d;
    updateValues();
  }

  void QSimControlWidget::setStepDeltaTime(const float& dt)
  {
    this->_stepDeltaTime = dt;
    this->getSimulationPlayer( )->deltaTime( this->_stepsPerSecond * this->_stepDeltaTime );

    if(_qSimPlayer->isPlaying()) this->_qSimPlayer->play( );
  }

  void QSimControlWidget::update( )
  {
    if ( this->isPlaying( ) )
    {
      this->_qSimPlayer->update( true );
    }
  }

  void QSimControlWidget::reset( void )
  {
    this->_qSimPlayer->reset( );
  }

  simil::SimulationPlayer * QSimControlWidget::getSimulationPlayer( void )
  {
    return this->_qSimPlayer->getSimulationPlayer( );
  }

  float QSimControlWidget::getStepsPerSecond( void ) const 
  {
    return this->_stepsPerSecond;
  }

  float QSimControlWidget::getStepDeltaTime( void ) const 
  {
    return this->_stepDeltaTime;
  }

  bool QSimControlWidget::isPlaying( void ) const 
  {
    return this->_qSimPlayer->isPlaying( );
  }

  void QSimControlWidget::init(simil::SimulationPlayer *player, bool autoStart)
  {
    _qSimPlayer->init(player, autoStart);

    _stepsPerSecond = 1.0;
    _stepDeltaTime = 0.25;
    updateValues();
  }

  void QSimControlWidget::onDeltaTimeChanged(double d)
  {
    _stepDeltaTime = d;
    updateValues();
  }

};
