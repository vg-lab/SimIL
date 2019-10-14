/*
 * @file  SpikesPlayer.h
 * @brief
 * @author Sergio E. Galindo <sergio.galindo@urjc.es>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 *          Do not distribute without further notice.
 */

#ifndef __SIMIL__SPIKESPLAYER_H__
#define __SIMIL__SPIKESPLAYER_H__

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
#include "SimulationPlayer.h"
#include "storage/StorageSparse.h"
#include "Spikes.hpp"

namespace simil
{
  typedef TSpikes::iterator SpikesIter;
  typedef TSpikes::const_iterator SpikesCIter;

  typedef std::pair< SpikesIter, SpikesIter > SpikesRange;
  typedef std::pair< SpikesCIter, SpikesCIter > SpikesCRange;

  class SpikesPlayer : public SimulationPlayer
  {
  public:

    SpikesPlayer( void );

    //virtual void LoadData( SimulationData* data );

    virtual void LoadData( TDataType dataType,
                           const std::string& networkPath,
                           const std::string& activityPath = "" );

    virtual void Clear( void );
    virtual void PlayAt( float percentage );
    virtual void Stop( void );

    virtual const Spikes& spikes( void );
    StorageSparse* spikeReport( void ) const;

    SpikesCRange spikesAtTime( float time );

    SpikesCRange spikesBetween( float startTime, float endTime );

    SpikesCRange spikesNow( void );

    void spikesNowVect( std::vector< uint32_t >& );

    StorageSparse* data( void ) const;

  protected:

    virtual void FrameProcess( void );

    SpikesCIter _previousSpike;
    SpikesCIter _currentSpike;

  };

} // namespace simil

#endif /* __SIMIL__SPIKESPLAYER_H__ */
