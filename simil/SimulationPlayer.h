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
#include <simil/api.h>

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

  class SIMIL_API SimulationPlayer
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

    virtual void PlayAtPercentage( float percentage );

    virtual void PlayAtTime( float time );

    virtual float GetRelativeTime( void );

    bool isFinished( void );

    bool isPlaying( void );

    virtual void deltaTime( float deltaTime );
    virtual float deltaTime( void );

    float startTime( void );

    float endTime( void );

    float currentTime( void ) const;

    void loop( bool loop );
    bool loop( void );

    const TGIDSet& gids( void ) const;
    unsigned int gidsSize() const;
    TPosVect positions( void ) const;

    TSimulationType simulationType( void ) const ;

    SimulationData* data( void ) const;

#ifdef SIMIL_USE_ZEROEQ

    ZeroEqEventsManager* zeqEvents() const;

    void connectZeq( const std::string& session );
    void connectZeq( std::shared_ptr<zeroeq::Subscriber> subscriber, std::shared_ptr<zeroeq::Publisher> publisher );

    void requestPlaybackAt( float percentage );
    void sendCurrentTimestamp( void );

#endif

  protected:

    virtual void _checkSimData( void );

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

#ifdef SIMIL_USE_ZEROEQ
    ZeroEqEventsManager* _zeqEvents;
#endif
    DataSet* _dataset;
    Network* _network;
    SimulationData* _simData;
  };

} // namespace simil

#endif /* __SIMIL__SIMULATIONPLAYER_H_ */
