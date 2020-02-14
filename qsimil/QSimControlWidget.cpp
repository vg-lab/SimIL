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

#include "QSimControlWidget.h"
#include <QGroupBox>
#include <QHBoxLayout>
#include <QDoubleSpinBox>

namespace qsimil
{
  QSimControlWidget::QSimControlWidget( QWidget *parent_)
    : QWidget(parent_)
    , _qSimPlayer( new QSimulationPlayer( ))
  {
    
    QGridLayout * grid = new QGridLayout( );
    this->setLayout(grid);

    QGroupBox* groupBox = new QGroupBox( tr("Simulation params") );
    groupBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    grid->addWidget(groupBox);

    QHBoxLayout* hbox = new QHBoxLayout();
    groupBox->setLayout(hbox);

    hbox->addWidget(new QLabel( tr( "StepsPerSecond" )));

    stepPerSecond = new QDoubleSpinBox( );
    hbox->addWidget( stepPerSecond );
    stepPerSecond->setSingleStep( 0.01 );

    connect( stepPerSecond, SIGNAL( valueChanged(const QString&)), 
      this, SLOT( onStepsPerSecondChanged(const QString& )));
    
    _deltaTime = new QLineEdit();
    _deltaTime->setText( "0.25" );
    hbox->addWidget(new QLabel( tr( "StepDeltaTime" ) ) );
    hbox->addWidget( _deltaTime );

    auto updateWindow = new QPushButton( tr( "Update" ) );
    hbox->addWidget( updateWindow );

    connect( updateWindow, SIGNAL ( released( ) ), 
      this, SLOT ( handleStepDeltaTimeUpdate() ) );


    QGroupBox* renderFuncMode = new QGroupBox( );
    renderFuncMode->setSizePolicy(QSizePolicy::Preferred, 
      QSizePolicy::Fixed);

    grid->addWidget(renderFuncMode);
    QVBoxLayout* vlayout = new QVBoxLayout;
    renderFuncMode->setLayout( vlayout );
    
    vlayout->addWidget( _qSimPlayer );
  }

  void QSimControlWidget::onStepsPerSecondChanged( const QString& s_ )
  {
    // std::cout << "VEL: " << s_.toFloat( ) << std::endl;
    QString v = s_ ; // s_.mid(1);  // Remove "x"
    float v_ = 1000.0 / v.toFloat( );
    this->setStepsPerSecond( v_ );
  }

  void QSimControlWidget::handleStepDeltaTimeUpdate( )
  {
    float value = this->_deltaTime->text().toFloat( );
    this->setStepDeltaTime( value );
  }

  void QSimControlWidget::init( const char* blueConfig, 
    simil::TSimulationType type, bool autoStart )
  {
    this->_qSimPlayer->init( blueConfig, type, autoStart );
    this->counterNewFrame.start( );
    this->_qSimPlayer->getSimulationPlayer( )->deltaTime( 
      this->getStepsPerSecond( ) );

    this->stepPerSecond->setValue( 1.00 );
    this->handleStepDeltaTimeUpdate();
  }

  void QSimControlWidget::setStepsPerSecond(const float& d)
  {
    this->_stepsPerSecond = d;
  }

  void QSimControlWidget::setStepDeltaTime(const float& dt)
  {
    this->_stepDeltaTime = dt;
    this->getSimulationPlayer( )->deltaTime( this->_stepDeltaTime );
    this->_qSimPlayer->play( );
  }

  void QSimControlWidget::update( )
  {
    if( counterNewFrame.elapsed( ) > getStepsPerSecond( ) )
    {
      counterNewFrame.restart( );

      if ( this->isPlaying( ) )
      {
        this->_qSimPlayer->update( true );
      }
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
};
