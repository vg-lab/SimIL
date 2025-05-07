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

#include "simil/Network.h"
#include <memory>

#ifdef SIMIL_USE_BRION
#include <brion/brion.h>
#include <brain/brain.h>
#endif

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

  typedef std::pair< SpikesIter , SpikesIter > SpikesRange;
  typedef std::pair< SpikesCIter , SpikesCIter > SpikesCRange;

  class SIMIL_API SpikesPlayer : public SimulationPlayer
  {
  public:

    SpikesPlayer( );

    void LoadData( std::shared_ptr< SimulationData > data ) override;

    void LoadData( std::shared_ptr< Network > network ,
                   std::shared_ptr< SimulationData > data ) override;

    void LoadData( TDataType dataType ,
                   const std::string& networkPath ,
                   const std::string& activityPath = "" ) override;

    void Clear( ) override;

    void PlayAtTime( float timePos ) override;

    void PlayAtPercentage( float percentage ) override;

    void Stop( ) override;

    virtual const Spikes& spikes( );

    unsigned int spikesSize( ) const;

    std::shared_ptr< SpikeData > spikeReport( void ) const;

    SpikesCRange spikesAtTime( float time );

    SpikesCRange spikesBetween( float startTime , float endTime );

    SpikesCRange spikesNow( );

    void spikesNowVect( std::vector< uint32_t >& );

    /** \brief Saves the spikes data in a CSV file.
     * \param[in] filename File name on disk of the CSV output file.
     * 
     */
    bool saveSpikesAsCSV(const std::string &filename);

  protected:
    void _checkSimData( ) override;

    void FrameProcess( ) override;

    SpikesCIter _previousSpike;
    SpikesCIter _currentSpike;

  };

} // namespace simil

#endif /* __SIMIL__SPIKESPLAYER_H__ */
