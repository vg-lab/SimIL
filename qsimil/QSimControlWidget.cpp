/*
 * ClickableSlider.cpp
 *
 *  Created on: 11 de dic. de 2015
 *  Updated on: 15 de sept. de 2016
 *      Author: sgalindo
 *      Author: crodriguez
 */

#include "QSimControlWidget.h"
#include <QGroupBox>
#include <QHBoxLayout>
#include <QDoubleSpinBox>

namespace qsimil
{
    QSimControlWidget::QSimControlWidget( QWidget *parent )
        : QWidget(parent)
        , _qSimPlayer( new QSimulationPlayer( ))
    {
        
        QGridLayout * grid = new QGridLayout( );
        this->setLayout(grid);

        QGroupBox* groupBox = new QGroupBox( tr("Simulation params") );
        groupBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

        grid->addWidget(groupBox);

        QHBoxLayout* vboxT = new QHBoxLayout();
        groupBox->setLayout(vboxT);

        vboxT->addWidget(new QLabel( tr( "StepsPerSecond" )));

        stepPerSecond = new QDoubleSpinBox( );
        vboxT->addWidget( stepPerSecond );
        stepPerSecond->setSingleStep( 0.01 );

        connect( stepPerSecond, SIGNAL( valueChanged(const QString&)), 
            this, SLOT( onStepsPerSecondChanged(const QString& )));
        
        _deltaTime = new QLineEdit();
        _deltaTime->setText( "0.25" );
        vboxT->addWidget(new QLabel( tr( "StepDeltaTime" ) ) );
        vboxT->addWidget( _deltaTime );

        auto updateWindow = new QPushButton( tr( "Update" ) );
        vboxT->addWidget( updateWindow );

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

    void QSimControlWidget::onStepsPerSecondChanged( QString s_ )
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

    void QSimControlWidget::setStepDeltaTime(const float& ws)
    {
        this->_stepDeltaTime = ws;
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

    inline float QSimControlWidget::getStepsPerSecond( void ) const 
    {
        return this->_stepsPerSecond;
    }
    inline float QSimControlWidget::getStepDeltaTime( void ) const 
    {
        return this->_stepDeltaTime;
    }
    inline bool QSimControlWidget::isPlaying( void ) const 
    {
        return this->_qSimPlayer->isPlaying( );
    }
};
