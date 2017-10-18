/*
 * SimulationData.h
 *
 *  Created on: 5 de abr. de 2016
 *      Author: sgalindo
 */

#ifndef __SIMIL__SIMULATIONDATA_H__
#define __SIMIL__SIMULATIONDATA_H__

#ifdef SIMIL_USE_BRION
#include <brion/brion.h>
#include <brain/brain.h>
#endif

#include <H5Cpp.h>

#include "types.h"
#include "H5Network.h"
#include "SubsetEventManager.h"
#include "Spikes.hpp"

namespace simil
{
  class SimulationData
  {
  public:

    SimulationData( std::string filePath, TDataType dataType );
    virtual ~SimulationData( void );

    const TGIDSet& gids( void ) const;

    GIDVec gidsVec( void ) const;

    const TPosVect& positions( void ) const;

    SubsetEventManager* subsetsEvents( void );

    TSimulationType simulationType( void ) const;

    virtual SimulationData* get( void );

    virtual float startTime( void ) const;
    virtual float endTime( void ) const;

  protected:

    std::string filePath;

    TGIDSet _gids;

    TPosVect _positions;

    simil::SubsetEventManager _subsetEventManager;

//    simil::SubsetMap _subsets;

    TSimulationType _simulationType;

#ifdef SIMIL_USE_BRION
    brion::BlueConfig* _blueConfig;
#endif
    H5Network* _h5Network;

    float _startTime;
    float _endTime;

  };

  class SpikeData : public SimulationData
  {
  public:

    SpikeData( const std::string& filePath, TDataType dataType,
               const std::string& report = "" );

    const Spikes& spikes( void ) const;

    SpikeData* get( void );

  protected:

    Spikes _spikes;
  };

  class VoltageData : public SimulationData
  {

    VoltageData( const std::string& filePath, TDataType dataType,
                 const std::string& report = ""  );

  };

}



#endif /* __SIMIL__SIMULATIONDATA_H__ */