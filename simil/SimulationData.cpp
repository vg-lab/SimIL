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

#include "SimulationData.h"



namespace simil
{
  SimulationData::SimulationData( const std::string& filePath_,
                                  TDataType dataType,
                                  const std::string& target )
  : _dataType( dataType )
  , _simulationType( TSimNetwork )
#ifdef SIMIL_USE_BRION
    , _blueConfig( nullptr )
  , _target( target )
#endif
  , _h5Network( nullptr )
  , _csvNetwork( nullptr )
  , _startTime( 0.0f )
  , _endTime( 0.0f )
  , _isDirty( false )
  {
    target.size( ); // TODO remove this workaround to unused variable error
    switch ( dataType )
    {
      case TBlueConfig:
      {
#ifdef SIMIL_USE_BRION
        _blueConfig = new brion::BlueConfig( filePath_ );
        brion::Targets targets = _blueConfig->getTargets( );

        brain::Circuit* circuit = new brain::Circuit( *_blueConfig );

        if ( !target.empty( ) )
          _gids = brion::Target::parse( targets, target );
        else
          _gids = circuit->getGIDs( );

        _positions = circuit->getPositions( _gids );

        delete circuit;
#else
        std::cerr << "Error: Brion support not available" << std::endl;
        exit( -1 );
#endif
        break;
      }
      case THDF5:
      {
        _h5Network = new H5Network( filePath_ );
        _h5Network->load( );

        _gids = _h5Network->getGIDs( );

        _positions = _h5Network->getComposedPositions( );

        auto subsetIts = _h5Network->getSubsets( );
        for ( simil::SubsetMapCIt it = subsetIts.first; it != subsetIts.second;
              ++it )
          _subsetEventManager.addSubset( it->first, it->second );

        break;
      }
      case TCSV:
      {
        _csvNetwork = new CSVNetwork( filePath_ );
        _csvNetwork->load( );

        _gids =  _csvNetwork->getGIDs( );

        _positions = _csvNetwork->getComposedPositions( );
        break;
      }
      default:
        break;
    }
  }

  SimulationData::SimulationData( )
    : _simulationType( TSimNetwork )
#ifdef SIMIL_USE_BRION
    , _blueConfig( nullptr )
#endif
    , _h5Network( nullptr )
    , _startTime( 0.0f )
    , _endTime( 0.0f )

  {
  }

  SimulationData::~SimulationData( void )
  {
  }

  void SimulationData::setGids( const TGIDSet& gids )
  {
    _gids = gids;
  }
  void SimulationData::setGid( const uint32_t gid )
  {
    _gids.insert(gid);
  }

  void SimulationData::setPositions( TPosVect positions )
  {
    _positions = positions;
  }
  void SimulationData::setPosition( vmml::Vector3f position )
  {
    _positions.push_back(position);
  }

  void SimulationData::setSubset( SubsetEventManager subsets )
  {
    _subsetEventManager = subsets;
  }

  void SimulationData::setSimulationType( TSimulationType s_type )
  {
    _simulationType = s_type;
  }

  void SimulationData::setStartTime( float startTime )
  {
    _isDirty = true;
    _startTime = startTime;
  }
  void SimulationData::setEndTime( float endTime )
  {
    _isDirty = true;
    _endTime = endTime;
  }

  const TGIDSet& SimulationData::gids( void ) const
  {
    return _gids;
  }

  GIDVec SimulationData::gidsVec( void ) const
  {
    return GIDVec( _gids.begin( ), _gids.end( ) );
  }

  const TPosVect& SimulationData::positions( void ) const
  {
    return _positions;
  }

  simil::SubsetEventManager* SimulationData::subsetsEvents( void )
  {
    return &_subsetEventManager;
  }


  const simil::SubsetEventManager* SimulationData::subsetsEvents( void ) const
  {
    return &_subsetEventManager;
  }

  TSimulationType SimulationData::simulationType( void ) const
  {
    return _simulationType;
  }

  SimulationData* SimulationData::get( void )
  {
    return this;
  }

  float SimulationData::startTime( void ) const
  {
    return _startTime;
  }

  float SimulationData::endTime( void ) const
  {
//    if( _dataType == THDF5 )
//      return std::max( _endTime, _subsetEventManager.totalTime( ));
//    else
      return _endTime;
  }

  bool SimulationData::isDirty( void ) const
  {
    return _isDirty;
  }
  void SimulationData::cleanDirty( void )
  {
    _isDirty = false;
  }


} // namespace simil
