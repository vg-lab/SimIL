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
#include "SimulationData.h"
#include "SpikeData.h"
#include "DataSet.h"
#include "SimulationPlayer.h"
#include <simil/api.h>

namespace simil
{
  typedef TSpikes::iterator SpikesIter;
  typedef TSpikes::const_iterator SpikesCIter;

  typedef std::pair< SpikesIter, SpikesIter > SpikesRange;
  typedef std::pair< SpikesCIter, SpikesCIter > SpikesCRange;

  class SIMIL_API SpikesPlayer : public SimulationPlayer
  {
  public:

    SpikesPlayer( void );

    virtual void LoadData( SimulationData* data );

    virtual void LoadData( Network* network,SimulationData* data);

    virtual void LoadData( TDataType dataType,
                           const std::string& networkPath,
                           const std::string& activityPath = "" );

    virtual void Clear( void );
    virtual void PlayAtTime(float timePos);
    virtual void PlayAtPercentage( float percentage );
    virtual void Stop( void );

    virtual const Spikes& spikes( void );
    unsigned int spikesSize() const;
    SpikeData* spikeReport( void ) const;

    SpikesCRange spikesAtTime( float time );

    SpikesCRange spikesBetween( float startTime, float endTime );

    SpikesCRange spikesNow( void );

    void spikesNowVect( std::vector< uint32_t >& );

    SpikeData* data( void ) const;

  protected:
    virtual void _checkSimData( void );

    virtual void FrameProcess( void );

    SpikesCIter _previousSpike;
    SpikesCIter _currentSpike;

  };

} // namespace simil

#endif /* __SIMIL__SPIKESPLAYER_H__ */
