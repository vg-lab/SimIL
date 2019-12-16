/*
 * @file  Network.cpp
 * @brief
 * @author Sergio E. Galindo <sergio.galindo@urjc.es>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 *          Do not distribute without further notice.
 */

#include "Network.h"



namespace simil
{
  Network::Network( const std::string& filePath_,
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

  Network::Network( )
    : _simulationType( TSimNetwork )
#ifdef SIMIL_USE_BRION
    , _blueConfig( nullptr )
#endif
    , _h5Network( nullptr )
  {
  }

  Network::~Network( void )
  {
  }


  void Network::setDataType( TDataType dataType )
  {
      _dataType = dataType;
  }
  TDataType Network::dataType( )
  {
      return _dataType;
  }

  void Network::setGids( const TGIDSet& gids )
  {
    _gids = gids;
  }

  void Network::setPositions( TPosVect positions )
  {
    _positions = positions;
  }

  void Network::setSubset( SubsetEventManager subsets )
  {
    _subsetEventManager = subsets;
  }

  void Network::setSimulationType( TSimulationType s_type )
  {
    _simulationType = s_type;
  }


  const TGIDSet& Network::gids( void ) const
  {
    return _gids;
  }

  GIDVec Network::gidsVec( void ) const
  {
    return GIDVec( _gids.begin( ), _gids.end( ) );
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
