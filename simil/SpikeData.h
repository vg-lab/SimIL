/*
 * @file  SimulationData.h
 * @brief
 * @author Sergio E. Galindo <sergio.galindo@urjc.es>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 *          Do not distribute without further notice.
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
#include "loaders/auxiliar/CSVActivity.h"

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
