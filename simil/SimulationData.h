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
#include "CSVNetwork.h"
#include "CSVActivity.h"

namespace simil
{
  class SimulationData
  {
  public:
	  SIMIL_API
    SimulationData( );

	SIMIL_API
    SimulationData( const std::string& filePath, TDataType dataType,
                    const std::string& target = "" );
	SIMIL_API
    virtual ~SimulationData( void );

	SIMIL_API
    void setGids( const TGIDSet& gids );
	SIMIL_API
    void setGid( const uint32_t gid );
	SIMIL_API
    const TGIDSet& gids( void ) const;
	SIMIL_API
    GIDVec gidsVec( void ) const;
	SIMIL_API
    const TPosVect& positions( void ) const;
	SIMIL_API
    void setPositions( TPosVect positions );
	SIMIL_API
    void setPosition( vmml::Vector3f positions );
	SIMIL_API
    void setSubset( SubsetEventManager subsets );
	SIMIL_API
    SubsetEventManager* subsetsEvents( void );
	SIMIL_API
    const SubsetEventManager* subsetsEvents( void ) const;
	SIMIL_API
    void setSimulationType( TSimulationType s_type );
	SIMIL_API
    TSimulationType simulationType( void ) const;
	SIMIL_API
    virtual SimulationData* get( void );
	SIMIL_API
    virtual float startTime( void ) const;
	SIMIL_API
    virtual float endTime( void ) const;
	SIMIL_API
    void setStartTime( float startTime );
	SIMIL_API
    void setEndTime( float endTime );

#ifdef SIMIL_USE_BRION
	SIMIL_API
    const brion::BlueConfig* blueConfig( void ) const;
	SIMIL_API
    const std::string& target( void ) const;
#endif

  protected:
    std::string filePath;

    TGIDSet _gids;

    TPosVect _positions;

    simil::SubsetEventManager _subsetEventManager;

    //    simil::SubsetMap _subsets;

    TDataType _dataType;
    TSimulationType _simulationType;

#ifdef SIMIL_USE_BRION
    brion::BlueConfig* _blueConfig;

    std::string _target;
#endif
    H5Network* _h5Network;

    CSVNetwork* _csvNetwork;

    float _startTime;
    float _endTime;
  };

  class SpikeData : public SimulationData
  {
  public:
	  SIMIL_API
    SpikeData( );
	  SIMIL_API
    SpikeData( const std::string& filePath, TDataType dataType,
               const std::string& report = "" );
	  SIMIL_API
    const Spikes& spikes( void ) const;
	  SIMIL_API
    void setSpikes( Spikes spikes );
	  SIMIL_API
    void addSpike(float timestamp, unsigned int gid);
	  SIMIL_API
    SpikeData* get( void );
	  SIMIL_API
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
