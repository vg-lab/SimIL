/*
 * @file  SimulationData.cpp
 * @brief
 * @author Sergio E. Galindo <sergio.galindo@urjc.es>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 *          Do not distribute without further notice.
 */

#include "SimulationData.h"

#include "H5Activity.h"

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
  , _startTime( 0.0f )
  , _endTime( 0.0f )
  {
    target.size( ); // TODO remove this workaround to unused variable error
    switch( dataType )
    {
      case TBlueConfig:
      {
#ifdef SIMIL_USE_BRION
        _blueConfig = new brion::BlueConfig( filePath_ );
        brion::Targets targets = _blueConfig->getTargets( );

        brain::Circuit* circuit = new brain::Circuit( *_blueConfig );

        if( !target.empty( ))
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

        _gids =  _h5Network->getGIDs( );

        _positions = _h5Network->getComposedPositions( );

        auto subsetIts = _h5Network->getSubsets( );
        for( simil::SubsetMapCIt it = subsetIts.first; it != subsetIts.second; ++it )
          _subsetEventManager.addSubset( it->first, it->second );

        break;
      }
      default:
        break;
    }
  }

  SimulationData::~SimulationData( void )
  {

  }

  const TGIDSet& SimulationData::gids( void ) const
  {
    return _gids;
  }

  GIDVec SimulationData::gidsVec( void ) const
  {
    return GIDVec( _gids.begin( ), _gids.end( ));
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

#ifdef SIMIL_USE_BRION
    const brion::BlueConfig* SimulationData::blueConfig( void ) const
    {
      return _blueConfig;
    }

    const std::string& SimulationData::target( void ) const
    {
      return _target;
    }

#endif



  SpikeData::SpikeData( const std::string& filePath_, TDataType dataType,
                        const std::string& report  )
  : SimulationData( filePath_, dataType, report )
  {

    _simulationType = simil::TSimSpikes;

    switch( dataType )
    {
      case TBlueConfig:
      {
#ifdef SIMIL_USE_BRION
        if( _blueConfig )
        {
          brain::SpikeReportReader spikeReport(  _blueConfig->getSpikeSource( ));
          _spikes = spikeReport.getSpikes(0, spikeReport.getEndTime( ));

          _startTime = 0.0f;
          _endTime = spikeReport.getEndTime( );
        }
#else
        std::cerr << "Error: Brion support not available" << std::endl;
        exit( -1 );
#endif
        break;
      }
      case THDF5:
      {
        if( report.empty( ))
        {
          std::cerr << "Error: Activity file path is empty." << std::endl;
        }

        H5Spikes spikeReport( *_h5Network, report );
        spikeReport.Load(  );

        _spikes = spikeReport.spikes( );

        _startTime = spikeReport.startTime( );
        _endTime = spikeReport.endTime( );

        break;
      }
      default:
        break;
    }

  }

  void SpikeData::reduceDataToGIDS( void )
  {
    std::cout << "Before: " << _spikes.size( ) << std::endl;
    TSpikes aux;
    aux.reserve( _spikes.size( ));
    for( auto spike : _spikes )
      if( _gids.find( spike.second ) != _gids.end( ))
        aux.push_back( spike );

    aux.shrink_to_fit( );

    _spikes = Spikes( aux );

    std::cout << "After: " << _spikes.size( ) << std::endl;
  }

  const Spikes& SpikeData::spikes( void ) const
  {
    return _spikes;
  }

  SpikeData* SpikeData::get( void )
  {
    return this;
  }


} // namespace simil


