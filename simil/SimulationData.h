/*
 * @file  SimulationData.h
 * @brief
 * @author Sergio E. Galindo <sergio.galindo@urjc.es>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 *          Do not distribute without further notice.
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
    SimulationData( );

    SimulationData( const std::string& filePath, TDataType dataType,
                    const std::string& target = "" );
    virtual ~SimulationData( void );

    void setGids( const TGIDSet& gids );
    const TGIDSet& gids( void ) const;

    GIDVec gidsVec( void ) const;

    const TPosVect& positions( void ) const;
    void setPositions( TPosVect positions );

    void setSubset( SubsetEventManager subsets );
    SubsetEventManager* subsetsEvents( void );

    void setSimulationType( TSimulationType s_type );
    TSimulationType simulationType( void ) const;

    virtual SimulationData* get( void );

    virtual float startTime( void ) const;
    virtual float endTime( void ) const;
    void setStartTime( float startTime );
    void setEndTime( float endTime );

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
    SpikeData( );
    SpikeData( const std::string& filePath, TDataType dataType,
               const std::string& report = "" );

    const Spikes& spikes( void ) const;
    void setSpikes( Spikes spikes );
    void addSpike(float timestamp, uint gid);
    SpikeData* get( void );

    void reduceDataToGIDS( void );

  protected:
    Spikes _spikes;
  };

  class VoltageData : public SimulationData
  {
    VoltageData( const std::string& filePath, TDataType dataType,
                 const std::string& report = "" );
  };

} // namespace simil

#endif /* __SIMIL__SIMULATIONDATA_H__ */
