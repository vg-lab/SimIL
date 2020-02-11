/*
 * @file  SimulationPlayer.cpp
 * @brief
 * @author Sergio E. Galindo <sergio.galindo@urjc.es>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 *          Do not distribute without further notice.
 */
#include "SpikesPlayer.h"
#include "log.h"
#include <exception>
#include <assert.h>
namespace simil
{


  SpikesPlayer::SpikesPlayer( void )
  : SimulationPlayer( )
  {
    _simulationType = TSimSpikes;
  }

  void SpikesPlayer::LoadData( SimulationData* data_ )
  {
    if( !data_ || !dynamic_cast< SpikeData* >( data_ )  )
      return;

    assert( ( data_->endTime( ) - data_->startTime( )) > 0 );

    Clear( );

    _simData = data_;

    _gids = _simData->gids( );

    std::cout << "GID Set size: " << _gids.size( ) << std::endl;

    SpikeData* spikeData = dynamic_cast< SpikeData* >( _simData );

    std::cout << "Loaded " << spikeData->spikes( ).size( ) << " spikes." << std::endl;

    _currentSpike = spikeData->spikes( ).begin( );
    _previousSpike = _currentSpike;

    _startTime = spikeData->startTime( );
    _endTime = spikeData->endTime( );

    _currentTime = _startTime;

    _invTimeRange = 1.0f / ( _endTime - _startTime);

  }

  void SpikesPlayer::LoadData( Network* net_ ,SimulationData* data_ )
  {
      if( !data_ || !dynamic_cast< SpikeData* >( data_ ) || !net_  )
        return;

      //assert( ( data_->endTime( ) - data_->startTime( )) > 0 );

      Clear( );

      _simData = data_;
      _network = net_;

      _gids = _network->gids( );

      std::cout << "GID Set size: " << _gids.size( ) << std::endl;

      SpikeData* spikeData = dynamic_cast< SpikeData* >( _simData );

      std::cout << "Loaded " << spikeData->spikes( ).size( ) << " spikes." << std::endl;

      _currentSpike = spikeData->spikes( ).begin( );
      _previousSpike = _currentSpike;

      _startTime = spikeData->startTime( );
      _endTime = spikeData->endTime( );

      _currentTime = _startTime;

    if (( _endTime - _startTime)>0)
        _invTimeRange = 1.0f / ( _endTime - _startTime);
    else
        _invTimeRange = 1.0f;
  }

  void SpikesPlayer::LoadData( TDataType dataType,
                               const std::string& networkPath,
                               const std::string& activityPath )
  {
    auto simData = new SpikeData( networkPath, dataType, activityPath );

    LoadData( simData );
  }

  void SpikesPlayer::Clear( void )
  {
    SimulationPlayer::Clear( );

    if( _simData )
    {
      delete _simData;
      _simData = nullptr;
    }

  }

  void SpikesPlayer::Stop( void )
  {
   _checkSimData();
    SimulationPlayer::Stop( );
    _currentSpike = spikes( ).begin( );
    _previousSpike = _currentSpike;
  }

  void SpikesPlayer::PlayAt( float percentage )
  {
    _checkSimData();
    SimulationPlayer::PlayAt( percentage );

    const Spikes& spikes_ = spikes( );

    _currentSpike = spikes_.begin( );
    _previousSpike = _currentSpike;

    _currentTime = percentage * ( _endTime - _startTime ) + _startTime;

    _currentSpike = spikes_.elementAt( _currentTime );

  }

  void SpikesPlayer::FrameProcess( void )
  {
    _checkSimData();
    if (_endTime == _startTime)
        return;
    const TSpikes& spikes_ = spikes( );
    _previousSpike = _currentSpike;
    SpikesCIter last;

    SpikesCIter spike = _currentSpike;
    while( ( *spike ).first  < _currentTime )
    {
      if( spike == spikes_.end( ))
      {
        _finished = true;
        Finished( );
        return;
      }
      last = spike;
      spike++;
    }
    _currentSpike = spike;
  }

  const Spikes& SpikesPlayer::spikes( void )
  {
    return dynamic_cast< SpikeData* >( _simData )->spikes( );
  }

  unsigned int SpikesPlayer::spikesSize() const
  {
      return dynamic_cast< SpikeData* >( _simData )->spikes( ).size();
  }


  SpikeData* SpikesPlayer::spikeReport( void ) const
  {
    return dynamic_cast< SpikeData* >( _simData );
  }

  SpikesCRange
  SpikesPlayer::spikesAtTime( float time )
  {
    return spikesBetween( time, time );
  }

  SpikesCRange SpikesPlayer::spikesBetween( float startTime_, float endTime_ )
  {
    _checkSimData();
    assert( endTime_ > startTime_ );

    const Spikes& spikes_ = spikes( );

    SpikesCIter begin = spikes_.end( );
    SpikesCIter end = spikes_.end( );

    begin = spikes_.elementAt( startTime_ );
    auto spike = begin;
    unsigned int spikesSize =  spikes_.size( );
    while( spike->first < endTime_ && spike - spikes_.begin( ) < spikesSize )
      ++spike;

    if( spike - spikes_.begin( ) >= spikesSize )
      spike = begin = spikes_.end( );

    end = spike;

    return std::make_pair( begin, end );

  }

  SpikesCRange SpikesPlayer::spikesNow( void )
  {
    return std::make_pair( _previousSpike, _currentSpike );
  }

  void SpikesPlayer::spikesNowVect( std::vector< uint32_t >& gidsv )
  {
    _checkSimData();
    auto spikes_ = this->spikesNow( );
    gidsv.resize( std::distance( spikes_.first, spikes_.second ));
    std::vector< uint32_t >::iterator resultIt = gidsv.begin( );
    for( auto& it = spikes_.first; it != spikes_.second; ++it, ++resultIt )
    {
      *resultIt = it->second;
    }
  }

  SpikeData* SpikesPlayer::data( void ) const
  {
    return dynamic_cast< SpikeData* >( _simData );
  }

  void SpikesPlayer::_checkSimData( void )
  {
      SpikeData* spikeData = static_cast< SpikeData* >( _simData );
      if (spikeData->isDirty())
      {
          std::cout << "Loaded " << spikeData->spikes( ).size( ) << " spikes." << std::endl;

          _currentSpike = spikeData->spikes( ).begin( );

          _startTime = spikeData->startTime( );
          _endTime = spikeData->endTime( );

            if (( _simData->endTime( ) - _simData->startTime( ))>0)
                _invTimeRange = 1.0f / ( _simData->endTime( ) - _simData->startTime( ));
            else
                _invTimeRange = 1.0f;
         spikeData->cleanDirty();
      }
  }



}
