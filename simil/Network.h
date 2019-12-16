/*
 * @file  Network.h
 * @brief
 * @author Sergio E. Galindo <sergio.galindo@urjc.es>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 *          Do not distribute without further notice.
 */

#ifndef __SIMIL__NETWORK_H__
#define __SIMIL__NETWORK_H__

#include "types.h"
#include "SubsetEventManager.h"
#include "H5Network.h"
#include "CSVNetwork.h"

#ifdef SIMIL_USE_BRION
#include <brion/brion.h>
#include <brain/brain.h>
#endif
#include <H5Cpp.h>

namespace simil
{
  class Network
  {
  public:
    Network( );

    Network( const std::string& filePath, TDataType dataType,
             const std::string& target = "" );
    virtual ~Network( void );

    void setGids( const TGIDSet& gids );
    const TGIDSet& gids( void ) const;

    void setDataType( TDataType dataType );
    TDataType dataType( );

    GIDVec gidsVec( void ) const;

    const TPosVect& positions( void ) const;
    void setPositions( TPosVect positions );

    void setSubset( SubsetEventManager subsets );
    SubsetEventManager* subsetsEvents( void );
    const SubsetEventManager* subsetsEvents( void ) const;

    void setSimulationType( TSimulationType s_type );
    TSimulationType simulationType( void ) const;

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
  };

} // namespace simil

#endif /* __SIMIL__NETWORK_H__ */
