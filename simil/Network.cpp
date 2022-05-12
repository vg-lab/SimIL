/*
 * Copyright (c) 2015-2020 VG-Lab/URJC.
 *
 * Authors: Aaron Sujar <aaron.sujar@urjc.es>
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

#include "Network.h"

namespace simil
{
  Network::Network( const std::string& filePath_, TDataType dataType,
                    const std::string& target )
    : _gidSize( 0 )
    , _dataType( dataType )
    , _simulationType( TSimNetwork )
    , _needUpdate( false )
#ifdef SIMIL_USE_BRION
    , _blueConfig( nullptr )
    , _target( target )
#endif
    , _h5Network( nullptr )
    , _csvNetwork( nullptr )
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

        _gids = _csvNetwork->getGIDs( );

        _positions = _csvNetwork->getComposedPositions( );
        break;
      }
      default:
        break;
    }
  }

  Network::Network( )
  : _gidSize( 0 )
  , _dataType( TDataUndefined )
  , _simulationType( TSimNetwork )
  , _needUpdate( false )
#ifdef SIMIL_USE_BRION
  , _blueConfig( nullptr )
#endif
  , _h5Network( nullptr )
  , _csvNetwork( nullptr )
  {
    _gids.insert( 0 );
    _positions.push_back( vmml::Vector3f( 0, 0, 0 ) );
    _gidsV.push_back( 0 );
    _gidSize = 1;
  }

  Network::~Network( void )
  {
  }

  bool Network::isUpdated( )
  {
    return _needUpdate;
  }

  void Network::setDataType( TDataType dataType )
  {
    _dataType = dataType;
  }

  TDataType Network::dataType( )
  {
    return _dataType;
  }

  void Network::setGids( const TGIDSet& gids, bool generatePos )
  {
    if ( gids.empty( ))
    {
      return;
    }

    unsigned int width = sqrt( gids.size( )+_gidSize );
    unsigned int i = 0;

    for (auto it_gids = gids.begin( ) ; it_gids != gids.end( ); ++it_gids )
    {
      unsigned int number = *it_gids;
      if ( _gids.count( number ) < 1 )
      {
        _gids.insert( ( number ) );
        _gidsV.push_back( number );

        if ( generatePos )
        {
          _positions.push_back( vmml::Vector3f( i % width, i / width, 0 ) );
          ++i;
        }
      }
    }

    if ( generatePos )
      _gidSize = _positions.size( );

    _needUpdate = true;
  }

  void Network::setPositions( TPosVect positions, bool append )
  {
    if ( append )
    {
      _positions.insert( _positions.begin( ), positions.begin( ),
                         positions.end( ) );
      _gidSize = _positions.size( );
    }
    else
    {
      _positions.clear( );
      _positions = positions;
      _gidSize = _positions.size( );
    }
    _needUpdate = true;
  }

  void Network::setNeurons( const TGIDVect& gids, const TPosVect& positions)
  {
    for (unsigned int i = 0; i < gids.size(); ++i)
    {
      unsigned int number = gids[i];
      if (_gids.count(number) < 1)
      {
        _gids.insert((number));
        _gidsV.push_back(number);
        _positions.push_back(positions[i]);

      }
    }

    _gidSize = _positions.size();
  }

  void Network::setSubset( SubsetEventManager subsets )
  {
    _subsetEventManager = subsets;
    _needUpdate = true;
  }

  void Network::setSimulationType( TSimulationType s_type )
  {
    _simulationType = s_type;
  }

  const TGIDSet& Network::gids( void )
  {
    _needUpdate = false;
    return _gids;
  }

  unsigned int Network::gidsSize( void )
  {
      return _gidSize;
  }

  const GIDVec& Network::gidsVec( void ) const
  {
    return _gidsV;
  }

  const TPosVect& Network::positions( void ) const
  {
    return _positions;
  }

  simil::SubsetEventManager* Network::subsetsEvents( void )
  {
    return &_subsetEventManager;
  }

  const simil::SubsetEventManager* Network::subsetsEvents( void ) const
  {
    return &_subsetEventManager;
  }

  TSimulationType Network::simulationType( void ) const
  {
    return _simulationType;
  }

} // namespace simil
