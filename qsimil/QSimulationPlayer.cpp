/*
 * CustomSlider.cpp
 *
 *  Created on: 11 de dic. de 2015
 *  Updated on: 15 de sept. de 2016
 *      Author: sgalindo
 *      Author: crodriguez
 */

#include "QSimulationPlayer.h"
#include <QGroupBox>
#include <QHBoxLayout>

namespace qsimil
{
    QSimulationPlayer::QSimulationPlayer( QWidget *parent )
        : QWidget(parent)
        , _q( new QCustomPlayer( ))
    {
        
        QGridLayout * grid = new QGridLayout( );
        this->setLayout(grid);

        QGroupBox* groupBox = new QGroupBox( tr("Simulation params") );
        groupBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

        grid->addWidget(groupBox);

        QHBoxLayout* vboxT = new QHBoxLayout();
        groupBox->setLayout(vboxT);

        vboxT->addWidget(new QLabel( tr( "StepsPerSecond" )));

        stepPerSecond = new QComboBox( );
        stepPerSecond->addItem( tr("x0.25") );
        stepPerSecond->addItem( tr("x0.5") );
        stepPerSecond->addItem( tr("x0.75") );
        stepPerSecond->addItem( tr("x1.0") );
        stepPerSecond->addItem( tr("x1.5") );
        stepPerSecond->addItem( tr("x2.0") );
        stepPerSecond->addItem( tr("x4.0") );
        stepPerSecond->addItem( tr("x8.0") );
        vboxT->addWidget( stepPerSecond );

        connect( stepPerSecond, SIGNAL( currentIndexChanged(const QString&)), 
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
        
        vlayout->addWidget( _q );
    }

    void QSimulationPlayer::onStepsPerSecondChanged( QString s_ )
    {
        QString v = s_.mid(1);
        float v_ = 1000.0 / v.toFloat( );
        this->setStepsPerSecond( v_ );
    }

    void QSimulationPlayer::handleStepDeltaTimeUpdate( )
    {
        float value = this->_deltaTime->text().toFloat( );
        this->setStepDeltaTime( value );
    }

    void QSimulationPlayer::init( ICustomPlayer* icp, 
        const char* blueConfig, bool autoStart )
    {
        this->_q->init( icp, blueConfig, autoStart );
        this->counterNewFrame.start( );
        this->_q->getSpikesPlayer( )->deltaTime( 
            this->getStepsPerSecond( ) );

        this->stepPerSecond->setCurrentIndex(3);
        this->handleStepDeltaTimeUpdate();
    }

    void QSimulationPlayer::setStepsPerSecond(const float& d)
    {
        this->_stepsPerSecond = d;
        this->_q->getICustomPlayer( )->cpChangeStepsPerSecond( 
            this->_stepsPerSecond);
    }

    void QSimulationPlayer::setStepDeltaTime(const float& ws)
    {
        this->_stepDeltaTime = ws;
        this->getSimulation()->deltaTime( this->_stepDeltaTime );
        this->_q->getICustomPlayer( )->cpChangeStepDeltaTime( 
            this->_stepDeltaTime);
    }

    void QSimulationPlayer::update( )
    {
        if( counterNewFrame.elapsed( ) > getStepsPerSecond( ) )
        {
            counterNewFrame.restart( );

            if ( this->isPlaying( ) )
            {
                this->_q->update( true );
            }
        }
    }

    void QSimulationPlayer::reset( void )
    {
        this->_q->reset( );
    }

    simil::SpikesPlayer * QSimulationPlayer::getSimulation( void )
    {
        return this->_q->getSpikesPlayer( );
    }
};
