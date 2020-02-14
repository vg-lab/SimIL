/*
 * Copyright (c) 2015-2020 GMRV/URJC.
 *
 * Authors: Sergio E. Galindo <sergio.galindo@urjc.es>
 *
 * This file is part of SimIL <https://github.com/gmrvvis/SimIL>
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

#ifndef __SIMIL__SPIKEDATA_H__
#define __SIMIL__SPIKEDATA_H__

#ifdef SIMIL_USE_BRION
#include <brion/brion.h>
#include <brain/brain.h>
#endif

#include "types.h"
#include "SimulationData.h"

#include "Spikes.hpp"
#include "loaders/aux/CSVActivity.h"

namespace simil
{


  class SpikeData : public SimulationData
  {
  public:
    SpikeData( );
    SpikeData( const std::string& filePath, TDataType dataType,
               const std::string& report = "" );

    const Spikes& spikes( void ) const;
    void setSpikes( Spikes spikes );
    void addSpikes(TSpikes & spikes);
    SpikeData* get( void );

    void reduceDataToGIDS( void );

  protected:
    Spikes _spikes;
  };



} // namespace simil

#endif /* __SIMIL__SPIKEDATA_H__ */
