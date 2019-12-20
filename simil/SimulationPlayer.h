/*
 * @file  SimulationPlayer.h
 * @brief
 * @author Sergio E. Galindo <sergio.galindo@urjc.es>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 *          Do not distribute without further notice.
 */

#ifndef __SIMIL__SIMULATIONPLAYER_H__
#define __SIMIL__SIMULATIONPLAYER_H__

#ifdef SIMIL_USE_BRION
#include <brion/brion.h>
#include <brain/brain.h>
#endif
#include <simil/api.h>

#include <unordered_map>

#ifdef SIMIL_USE_ZEROEQ

#include <boost/signals2/signal.hpp>
#include <boost/bind.hpp>
#include "ZeroEqEventsManager.h"
#endif

#include "types.h"
#include "SimulationData.h"
#include "DataSet.h"

namespace simil
{

  typedef enum
  {
    T_PLAYBACK_PLAY = 0,
    T_PLAYBACK_PAUSE,
    T_PLAYBACK_STOP,
    T_PLAYBACK_RESTART,
    T_PLAYBACK_REPEAT,
    T_PLAYBACK_SINGLE_SHOT

  } TPlaybackOperation;



  class SimulationPlayer
  {

  public:

    SimulationPlayer( void );

    virtual ~SimulationPlayer( );

    virtual void LoadData( SimulationData* data );

    virtual void LoadData(DataSet*);

    virtual void LoadData( Network* network,SimulationData* data);

    virtual void LoadData( TDataType dataType,
                           const std::string& networkPath,
                           const std::string& activityPath = "" );

    virtual void Clear( void );

    virtual void Frame( void );

    virtual void Play( void );

    virtual void Reset( void );

    virtual void Pause( void );

    virtual void Stop( void );

    virtual void GoTo( float timeStamp );

    virtual void PlayAt( float percentage );

    virtual float GetRelativeTime( void );

    bool isFinished( void );

    bool isPlaying( void );

    virtual void deltaTime( float deltaTime );
    virtual float deltaTime( void );

    float startTime( void );

    float endTime( void );

    float currentTime( void );

    void loop( bool loop );
    bool loop( void );

    const TGIDSet& gids( void ) const;
    TPosVect positions( void ) const;

    TSimulationType simulationType( void ) const ;

    SimulationData* data( void ) const;

#ifdef SIMIL_USE_ZEROEQ

    ZeroEqEventsManager* zeqEvents( void );

    void connectZeq( const std::string& zeqUri );

    void requestPlaybackAt( float percentage );
    void sendCurrentTimestamp( void );

#endif

  protected:

    virtual void FrameProcess( void ) = 0;
    virtual void Finished( void );

    float _currentTime;
    float _previousTime;
    float _relativeTime;
    float _invTimeRange;

    float _deltaTime;

    float _startTime;
    float _endTime;

    bool _playing;
    bool _loop;
    bool _finished;

    TSimulationType _simulationType;

    TGIDSet _gids;

#ifdef SIMIL_USE_ZEROEQ
    ZeroEqEventsManager* _zeqEvents;
#endif
    DataSet* _dataset;
    Network* _network;
    SimulationData* _simData;


  };

  typedef TSpikes::iterator SpikesIter;
  typedef TSpikes::const_iterator SpikesCIter;

  typedef std::pair< SpikesIter, SpikesIter > SpikesRange;
  typedef std::pair< SpikesCIter, SpikesCIter > SpikesCRange;

  class SpikesPlayer : public SimulationPlayer
  {
  public:

    SpikesPlayer( void );

    virtual void LoadData( SimulationData* data );

    virtual void LoadData( Network* network,SimulationData* data);

    virtual void LoadData( TDataType dataType,
                           const std::string& networkPath,
                           const std::string& activityPath = "" );

    virtual void Clear( void );
    virtual void PlayAt( float percentage );
    virtual void Stop( void );

    virtual const Spikes& spikes( void );
    SpikeData* spikeReport( void ) const;

    SpikesCRange spikesAtTime( float time );

    SpikesCRange spikesBetween( float startTime, float endTime );

    SpikesCRange spikesNow( void );

    void spikesNowVect( std::vector< uint32_t >& );

    SpikeData* data( void ) const;

  protected:

    virtual void FrameProcess( void );

    void Update();

    SpikesCIter _previousSpike;
    SpikesCIter _currentSpike;

  };

} // namespace simil

#endif /* __SIMIL__SIMULATIONPLAYER_H_ */
