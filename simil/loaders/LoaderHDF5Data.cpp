/*
 * @file  LoadHDF5Data.h
 * @brief
 * @author Aaron Sújar <aaron.sujar@urjc.es>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 *          Do not distribute without further notice.
 */

#include "LoaderHDF5Data.h"

//#include "../storage/StorageSparse.h"

namespace simil
{
  LoaderHDF5Data::LoaderHDF5Data( )
    : LoaderSimData( )
    , _h5Network( nullptr )
  {
  }

  LoaderHDF5Data::~LoaderHDF5Data( )
  {
    if ( _h5Network != nullptr )
      delete _h5Network;
  }

  /*DataSet* LoadHDF5Data::LoadNetwork( const std::string& filePath_,
                                      const std::string& target )
  {
    DataSet* dataset = new DataSet( filePath_, THDF5, target );

    if ( _h5Network == nullptr )
    {
      _h5Network = new H5Network( filePath_ );
      _h5Network->load( );
    }

    dataset->setGids( _h5Network->getGIDs( ) );

    dataset->setPositions( _h5Network->getComposedPositions( ) );

    SubsetEventManager subsetEventManager;
    auto subsetIts = _h5Network->getSubsets( );
    for ( simil::SubsetMapCIt it = subsetIts.first; it != subsetIts.second;
          ++it )
      subsetEventManager.addSubset( it->first, it->second );

    dataset->setSubset( subsetEventManager );

    return dataset;
  }*/

  SimulationData*
    LoaderHDF5Data::loadSimulationData( const std::string& filePath_,
                                        const std::string& target )
  {
    SpikeData* simulationdata = new SpikeData( );

    if ( _h5Network == nullptr )
    {
      _h5Network = new H5Network( filePath_ );
      _h5Network->load( );
    }

    simulationdata->setGids( _h5Network->getGIDs( ) );

    simulationdata->setPositions( _h5Network->getComposedPositions( ) );

    SubsetEventManager subsetEventManager;
    auto subsetIts = _h5Network->getSubsets( );
    for ( simil::SubsetMapCIt it = subsetIts.first; it != subsetIts.second;
          ++it )
      subsetEventManager.addSubset( it->first, it->second );

    simulationdata->setSubset( subsetEventManager );

    simil::H5Spikes spikeReport( *_h5Network, target );
    spikeReport.Load( );

    /*simil::StorageSparse* newStorage =
      new StorageSparse( "Spikes", tTYPE_UINT, TSimSpikes );*/

    simulationdata->setSimulationType( TSimSpikes );
    simulationdata->setSpikes( spikeReport.spikes( ) );
    simulationdata->setStartTime( spikeReport.startTime( ) );
    simulationdata->setEndTime( spikeReport.endTime( ) );

    // simulationdata->addStorage( newStorage );

    return simulationdata;
  }

} // namespace simil
