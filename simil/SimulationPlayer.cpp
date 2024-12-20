/*
 * Copyright (c) 2015-2020 VG-Lab/URJC.
 *
 * Authors: Sergio E. Galindo <sergio.galindo@urjc.es>
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

// SimIL
#include "SimulationPlayer.h"
#include "SimulationData.h"
#include "log.h"
#include "DataSet.h"

// C++
#include <exception>
#include <assert.h>
#include <memory>

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
    , _network( nullptr )
    , _simData( nullptr )
  { }

  SimulationPlayer::~SimulationPlayer( )
  {
    Clear( );
  }

  void SimulationPlayer::LoadData( std::shared_ptr< SimulationData > data_ )
  {
    if ( !data_ )
      return;

    assert(( data_->endTime( ) - data_->startTime( )) > 0 );

    Clear( );

    _simData = data_;

    std::cout << "GID Set size: " << gids( ).size( ) << std::endl;

    _invTimeRange = 1.0f / ( _simData->endTime( ) - _simData->startTime( ));
    _currentTime = _simData->startTime();
  }

  void SimulationPlayer::LoadData( const DataSet& dataset_ )
  {
    Clear( );
    LoadData( dataset_.network( ) , dataset_.simulationData( ));
    _currentTime = _simData->startTime();
  }

  void SimulationPlayer::LoadData( std::shared_ptr< Network > network ,
                                   std::shared_ptr< SimulationData > data )
  {
    if ( !network || !data )
      return;

    _network = network;
    _simData = data;

    std::cout << "GID Set size: " << gids( ).size( ) << std::endl;

    const auto timeDiff = _simData->endTime( ) - _simData->startTime( );
    if ( timeDiff > 0 )
      _invTimeRange = 1.0f / timeDiff;
    else
      _invTimeRange = 1.0f;

    _currentTime = _simData->startTime();
  }

  void SimulationPlayer::LoadData( TDataType dataType ,
                                   const std::string& networkPath_ ,
                                   const std::string& )
  {
    switch ( dataType )
    {
      case TDataType::TBlueConfig:
      case TDataType::THDF5:
      {
        _simData = std::make_shared< SimulationData >( networkPath_ ,
                                                       dataType );
      }
        break;
      default:
        break;
    }

    LoadData( _simData );
  }

  void SimulationPlayer::Clear( void )
  {
    _simData = nullptr;
    _network = nullptr;
  }

  void SimulationPlayer::Frame( void )
  {
    _checkSimData( );
    if ( _playing )
    {
      _previousTime = _currentTime;
      _currentTime += _deltaTime;

      _relativeTime = ( _currentTime - startTime( )) * _invTimeRange;

      FrameProcess( );
    }
  }

  void SimulationPlayer::Reset( void )
  {
    _checkSimData( );
    Stop( );
    Play( );
  }

  void SimulationPlayer::Play( void )
  {
    _checkSimData( );
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
    timeStamp = std::max( _startTime , std::min( timeStamp , _endTime ));

    _currentTime = timeStamp;
    _previousTime = std::max( _currentTime - _deltaTime , _startTime );

    _relativeTime = ( _currentTime - _startTime ) * _invTimeRange;
  }

  void SimulationPlayer::PlayAtPercentage( float percentage )
  {
    assert( percentage >= 0.0f && percentage <= 1.0f );

    const float timeStamp = percentage * ( _endTime - _startTime ) + _startTime;

    PlayAtTime( timeStamp );
  }

  void SimulationPlayer::PlayAtTime( float time )
  {
    time = std::max(_startTime, std::min(time, _endTime));

    _currentTime = time;
    _previousTime = std::max( _currentTime - _deltaTime , _startTime );

    _relativeTime = ( _currentTime - _startTime ) * _invTimeRange;

    Play( );
  }

  float SimulationPlayer::GetRelativeTime( void )
  {
    _checkSimData( );
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
    _checkSimData( );
    return _startTime;
  }

  float SimulationPlayer::endTime( void )
  {
    _checkSimData( );
    return _endTime;
  }

  float SimulationPlayer::currentTime( void ) const
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
    if ( _network )
      return _network->gids( );

    return _simData->gids( );
  }

  unsigned int SimulationPlayer::gidsSize( ) const
  {
    return gids( ).size( );
  }

  TPosVect SimulationPlayer::positions( void ) const
  {
    if ( _network )
      return _network->positions( );

    return _simData->positions( );
  }

  TSimulationType SimulationPlayer::simulationType( void ) const
  {
    return _simulationType;
  }

  std::shared_ptr<Network> SimulationPlayer::getNetwork() const
  {
    return _network;
  }

  void SimulationPlayer::Finished( void )
  {
    Stop( );
    if ( _loop )
    {
      _checkSimData( );
      Play( );
    }
  }

  const std::shared_ptr< SimulationData >& SimulationPlayer::data( void ) const
  {
    return _simData;
  }

  void SimulationPlayer::_checkSimData( void )
  {
    if ( _simData && _simData->isDirty( ))
    {
      const auto timeDiff = _simData->endTime( ) - _simData->startTime( );
      if ( timeDiff > 0 )
        _invTimeRange = 1.0f / timeDiff;
      else
        _invTimeRange = 1.0f;

      _startTime = _simData->startTime( );
      _endTime = _simData->endTime( );

      _relativeTime = ( _currentTime - _startTime ) * _invTimeRange;

      _simData->cleanDirty( );
    }
  }

#ifdef SIMIL_USE_ZEROEQ

  ZeroEqEventsManager* SimulationPlayer::zeqEvents() const
  {
    return _zeqEvents;
  }

  void SimulationPlayer::connectZeq( const std::string& session )
  {
    try
    {
      _zeqEvents = new ZeroEqEventsManager( session );

      _zeqEvents->frameReceived.connect( boost::bind( &SimulationPlayer::requestPlaybackAt,
                                         this, _1 ));
    }
    catch(const std::exception &e)
    {
      _zeqEvents = nullptr;
      throw;
    }
  }

  void SimulationPlayer::connectZeq( std::shared_ptr<zeroeq::Subscriber> subscriber,
                                     std::shared_ptr<zeroeq::Publisher> publisher)
  {
    try
    {
      _zeqEvents = new ZeroEqEventsManager( subscriber, publisher );

      _zeqEvents->frameReceived.connect( boost::bind( &SimulationPlayer::requestPlaybackAt,
                                         this, _1 ));
    }
    catch(const std::exception &e)
    {
      _zeqEvents = nullptr;
      throw;
    }
  }

  void SimulationPlayer::requestPlaybackAt( float position )
  {
    PlayAtTime( position );
  }

  void SimulationPlayer::sendCurrentTimestamp( void )
  {
    if( _playing && _zeqEvents)
      _zeqEvents->sendFrame( _startTime, _endTime, _currentTime );
  }
#endif

}
