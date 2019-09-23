/*
 * @file  SimulationPlayer.cpp
 * @brief
 * @author Sergio E. Galindo <sergio.galindo@urjc.es>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 *          Do not distribute without further notice.
 */
#include "SimulationPlayer.h"
#include "log.h"
#include <exception>
#include <assert.h>
#include "loaders/LoadHDF5Data.h"
#include "loaders/LoadblueConfigData.h"
namespace simil
{

  SimulationPlayer::SimulationPlayer( void )
  : _currentTime( 0.0f )
  , _previousTime( 0.0f )
  , _relativeTime( 0.0f )
  , _invTimeRange( 1.0f )
  , _deltaTime( 0.0f )
  , _startTime( 0.0f )
  , _endTime( 0.0f )
  , _playing( false )
  , _loop( false )
  , _finished( false )
  , _simulationType( TSimNetwork )
#ifdef SIMIL_USE_ZEROEQ
  , _zeqEvents( nullptr )
#endif
  , _simData( nullptr )
  , _simDataSet(nullptr)
  { }

  SimulationPlayer::~SimulationPlayer( )
  {
    Clear( );
  }

/*Deprecated*/
  void SimulationPlayer::LoadData( SimulationData* data_ )
  {
    if( !data_ )
      return;

    assert( ( data_->endTime( ) - data_->startTime( )) > 0 );

    Clear( );


    //_gids = _simData->gids( );


    std::cout << "GID Set size: " << _gids.size( ) << std::endl;

    _invTimeRange = 1.0f / ( _simData->endTime( ) - _simData->startTime( ));
  }

  void SimulationPlayer::LoadData( DataSet* data_ )
  {
    if( !data_ )
      return;

    _simDataSet = data_;

    _simData = _simDataSet->get(0);

    assert( ( _simData->endTime( ) - _simData->startTime( )) > 0 );

    Clear( );

    _gids = _simDataSet->gids( );

    std::cout << "GID Set size: " << _gids.size( ) << std::endl;

    _invTimeRange = 1.0f / ( _simData->endTime( ) - _simData->startTime( ));
  }

  void SimulationPlayer::LoadData( TDataType dataType,
                                   const std::string& networkPath_,
                                   const std::string& secondaryPath)
  {
    simil::LoadSimData* importer;

    switch( dataType )
    {
      case TDataType::TBlueConfig:
      {
        importer = new simil::LoadblueConfigData();
        break;
      }
      case TDataType::THDF5:
      {
        importer = new simil::LoadHDF5Data();
        //_simData = new SimulationData( networkPath_, dataType );
        break;
      }
      

      default:
        break;
    }

    _simDataSet = importer->LoadNetwork(networkPath_,secondaryPath);
    _simData = importer->LoadSimulationData(networkPath_,secondaryPath);
    _simDataSet->setSimulationData(_simData);
    LoadData( _simDataSet );
  }

  void SimulationPlayer::Clear( void )
  {
    if( _simDataSet )
    {
      delete _simData;
      _simData = nullptr;
      _simDataSet = nullptr;
    }

    _gids.clear( );
  }

  void SimulationPlayer::Frame( void )
  {
    if( _playing )
    {
      _previousTime = _currentTime;
      _currentTime += _deltaTime;

      _relativeTime = ( _currentTime - startTime( )) * _invTimeRange ;

      FrameProcess( );
    }
  }

  void SimulationPlayer::Reset( void )
  {
    Stop( );
    Play( );
  }


  void SimulationPlayer::Play( void )
  {
    _playing = true;
    _finished = false;
  }

  void SimulationPlayer::Pause( void )
  {
    _playing = false;
  }

  void SimulationPlayer::Stop( void )
  {
    _playing = false;
    _currentTime = _startTime;
    _previousTime = _currentTime;
  }

  void SimulationPlayer::GoTo( float timeStamp )
  {
    int aux = timeStamp / _deltaTime;

    _currentTime = aux * _deltaTime;
    _previousTime = std::max( _currentTime - _deltaTime, _startTime );

    _relativeTime = ( _currentTime - _startTime ) * _invTimeRange ;

  }

  void SimulationPlayer::PlayAt( float percentage )
  {
    assert( percentage >= 0.0f && percentage <= 1.0f );

    float timeStamp = percentage * ( _endTime - _startTime ) + _startTime;

    int aux = timeStamp / _deltaTime;

    _currentTime = aux * _deltaTime;
    _previousTime = std::max( _currentTime - _deltaTime, _startTime );

    _relativeTime = percentage;

    Play( );

  }

  float SimulationPlayer::GetRelativeTime( void )
  {
    return _relativeTime;
  }

  bool SimulationPlayer::isFinished( void )
  {
    return _finished;
  }

  bool SimulationPlayer::isPlaying( void )
  {
    return _playing;
  }

  void SimulationPlayer::deltaTime( float deltaTime_ )
  {
    _deltaTime = deltaTime_;
  }

  float SimulationPlayer::deltaTime( void )
  {
    return _deltaTime;
  }

  float SimulationPlayer::startTime( void )
  {
    return _startTime;
  }

  float SimulationPlayer::endTime( void )
  {
    return _endTime;
  }

  float SimulationPlayer::currentTime( void )
  {
    return _currentTime;
  }

  void SimulationPlayer::loop( bool loop_ )
  {
    _loop = loop_;
  }
  bool SimulationPlayer::loop( void )
  {
    return _loop;
  }

  const TGIDSet& SimulationPlayer::gids( void ) const
  {
    return _gids;
  }

  TPosVect SimulationPlayer::positions( void ) const
  {
    return _simDataSet->positions( );
  }

  TSimulationType SimulationPlayer::simulationType( void ) const
  {
    return _simulationType;
  }

  void SimulationPlayer::Finished( void )
  {
    Stop( );
    std::cout << "Finished simulation." << std::endl;
    if( _loop )
    {
      Play( );
    }
  }

  SimulationData* SimulationPlayer::data( void ) const
  {
    return _simData;
  }

#ifdef SIMIL_USE_ZEROEQ

  ZeroEqEventsManager* SimulationPlayer::zeqEvents( void )
  {
    return _zeqEvents;
  }


  void SimulationPlayer::connectZeq( const std::string& zeqUri )
  {
    _zeqEvents = new ZeroEqEventsManager( zeqUri );

    _zeqEvents->frameReceived.connect( boost::bind( &SimulationPlayer::requestPlaybackAt,
                                       this, _1 ));
  }

  void SimulationPlayer::requestPlaybackAt( float percentage )
  {
    PlayAt( percentage );
  }

  void SimulationPlayer::sendCurrentTimestamp( void )
  {
    if( _playing )
      _zeqEvents->sendFrame( _startTime, _endTime, _currentTime );
  }

#endif

}