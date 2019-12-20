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
  , _dataset( nullptr )
  , _network( nullptr )
  , _simData( nullptr )
  { }

  SimulationPlayer::~SimulationPlayer( )
  {
    Clear( );
  }

  void SimulationPlayer::LoadData( SimulationData* data_ )
  {
    if( !data_ )
      return;

    assert( ( data_->endTime( ) - data_->startTime( )) > 0 );

    Clear( );

    _gids = _simData->gids( );

    std::cout << "GID Set size: " << _gids.size( ) << std::endl;

    _invTimeRange = 1.0f / ( _simData->endTime( ) - _simData->startTime( ));
  }

  void SimulationPlayer::LoadData(DataSet * dataset_)
  {

      if( !dataset_ )
        return;

      Clear( );

      _dataset = dataset_;
      LoadData(dataset_->network(),dataset_->simulationData());
  }

  void SimulationPlayer::LoadData( Network* net_ ,SimulationData* data_ )
  {
    if( !net_ || !data_)
      return;
    _network = net_;
    _simData = data_;

    _gids = net_->gids( );

    std::cout << "GID Set size: " << _gids.size( ) << std::endl;
    if (( data_->endTime( ) - data_->startTime( ))>0)
        _invTimeRange = 1.0f / ( _simData->endTime( ) - _simData->startTime( ));
    else
        _invTimeRange = 1.0f;
  }

  void SimulationPlayer::LoadData( TDataType dataType,
                                   const std::string& networkPath_,
                                   const std::string& )
  {

    switch( dataType )
    {
      case TDataType::TBlueConfig:
      case TDataType::THDF5:
      {
        _simData = new SimulationData( networkPath_, dataType );
      }
      break;

      default:
        break;
    }

    LoadData( _simData );
  }

  void SimulationPlayer::Clear( void )
  {
    if( _simData )
    {
      delete _simData;
      _simData = nullptr;
    }

    if (_network)
    {
        delete _network;
        _network = nullptr;
    }

    if (_dataset)
    {
        delete _dataset;
        _dataset = nullptr;
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

//    int aux = timeStamp / _deltaTime;

    _currentTime = timeStamp;
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
    if (_network)
      return _network->positions( );

    return _simData->positions( );
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


  //*************************************************************************
  //************************ SPIKES SIMULATION PLAYER ***********************
  //*************************************************************************

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

    _invTimeRange = 1.0f / ( _simData->endTime( ) - _simData->startTime( ));

  }

  void SpikesPlayer::LoadData( Network* net_ ,SimulationData* data_ )
  {
      if( !data_ || !dynamic_cast< SpikeData* >( data_ ) || !net_  )
        return;

      assert( ( data_->endTime( ) - data_->startTime( )) > 0 );

      Clear( );

      _simData = data_;
      _network = net_;

      _gids = net_->gids( );

      std::cout << "GID Set size: " << _gids.size( ) << std::endl;

      SpikeData* spikeData = dynamic_cast< SpikeData* >( _simData );

      std::cout << "Loaded " << spikeData->spikes( ).size( ) << " spikes." << std::endl;

      _currentSpike = spikeData->spikes( ).begin( );
      _previousSpike = _currentSpike;

      _startTime = spikeData->startTime( );
      _endTime = spikeData->endTime( );

      _currentTime = _startTime;

    if (( data_->endTime( ) - data_->startTime( ))>0)
        _invTimeRange = 1.0f / ( _simData->endTime( ) - _simData->startTime( ));
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
    SimulationPlayer::Stop( );
    _currentSpike = spikes( ).begin( );
    _previousSpike = _currentSpike;
  }

  void SpikesPlayer::PlayAt( float percentage )
  {
      Update();
    SimulationPlayer::PlayAt( percentage );

    const Spikes& spikes_ = spikes( );

    _currentSpike = spikes_.begin( );
    _previousSpike = _currentSpike;

    _currentTime = percentage * ( _endTime - _startTime ) + _startTime;

    _currentSpike = spikes_.elementAt( _currentTime );

  }

  void SpikesPlayer::FrameProcess( void )
  {
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
    Update();
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

  void SpikesPlayer::Update()
  {
      SpikeData* spikeData = static_cast< SpikeData* >( _simData );

      std::cout << "Loaded " << spikeData->spikes( ).size( ) << " spikes." << std::endl;

      _currentSpike = spikeData->spikes( ).begin( );

      _startTime = spikeData->startTime( );
      _endTime = spikeData->endTime( );

    if (( _simData->endTime( ) - _simData->startTime( ))>0)
        _invTimeRange = 1.0f / ( _simData->endTime( ) - _simData->startTime( ));
    else
        _invTimeRange = 1.0f;
  }

}
