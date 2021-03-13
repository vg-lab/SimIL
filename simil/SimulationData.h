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

#ifndef __SIMIL__SIMULATIONDATA_H__
#define __SIMIL__SIMULATIONDATA_H__

#ifdef SIMIL_USE_BRION
#include <brion/brion.h>
#include <brain/brain.h>
#endif
#include <H5Cpp.h>


#include "types.h"
#include "SubsetEventManager.h"
#include "loaders/auxiliar/H5Network.h"
#include "loaders/auxiliar/CSVNetwork.h"
#include <simil/api.h>

namespace simil
{
  class SIMIL_API SimulationData
  {
  public:
    SimulationData( );

    SimulationData( const std::string& filePath, TDataType dataType,
                    const std::string& target = "" );
    virtual ~SimulationData( void );

    void setGids( const TGIDSet& gids );
    void setGid( const uint32_t gid );
    const TGIDSet& gids( void ) const;

    GIDVec gidsVec( void ) const;

    const TPosVect& positions( void ) const;
    void setPositions( TPosVect positions );
    void setPosition( vmml::Vector3f position );

    void setSubset( SubsetEventManager subsets );
    SubsetEventManager* subsetsEvents( void );
    const SubsetEventManager* subsetsEvents( void ) const;

    void setSimulationType( TSimulationType s_type );
    TSimulationType simulationType( void ) const;

    virtual SimulationData* get( void );

    virtual float startTime( void ) const;
    virtual float endTime( void ) const;
    void setStartTime( float startTime );
    void setEndTime( float endTime );

    bool isDirty( void ) const;
    void cleanDirty( void );


#ifdef SIMIL_USE_BRION
    const brion::BlueConfig* blueConfig( void ) const;

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

    bool _isDirty;
  };


  class VoltageData : public SimulationData
  {
    VoltageData( const std::string& filePath, TDataType dataType,
                 const std::string& report = "" );
  };

} // namespace simil

#endif /* __SIMIL__SIMULATIONDATA_H__ */
