/*
 * Copyright (c) 2015-2020 GMRV/URJC.
 *
 * Authors: Aaron Sujar <aaron.sujar@urjc.es>
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

#ifndef __SIMIL__NETWORK_H__
#define __SIMIL__NETWORK_H__

#include "types.h"
#include "SubsetEventManager.h"
#include "loaders/auxiliar/H5Network.h"
#include "loaders/auxiliar/CSVNetwork.h"
#include <simil/api.h>

#ifdef SIMIL_USE_BRION
#include <brion/brion.h>
#include <brain/brain.h>
#endif


namespace simil
{
  class SIMIL_API Network
  {
  public:
    Network( );

    Network( const std::string& filePath, TDataType dataType,
             const std::string& target = "" );
    virtual ~Network( void );

    bool isUpdated();

    void setGids( const TGIDSet& gids, bool generatePos = false );
    const TGIDSet& gids( void );

    unsigned int gidsSize( void );

    void setDataType( TDataType dataType );
    TDataType dataType( );

    const GIDVec& gidsVec( void ) const;

    const TPosVect& positions( void ) const;
    void setPositions( TPosVect positions, bool append = false );

    void setNeurons( const TGIDVect& gids, const TPosVect& positions);

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
    TGIDVect _gidsV;
    TPosVect _positions;

    unsigned int _gidSize;

    simil::SubsetEventManager _subsetEventManager;

    //    simil::SubsetMap _subsets;

    TDataType _dataType;
    TSimulationType _simulationType;

    bool _needUpdate;

#ifdef SIMIL_USE_BRION
    brion::BlueConfig* _blueConfig;

    std::string _target;
#endif
    H5Network* _h5Network;

    CSVNetwork* _csvNetwork;
  };

} // namespace simil

#endif /* __SIMIL__NETWORK_H__ */
