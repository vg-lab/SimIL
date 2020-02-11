/*
 * @file  LoadHDF5Data.h
 * @brief
 * @author Aaron Sújar <aaron.sujar@urjc.es>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 *          Do not distribute without further notice.
 */

#include "LoaderHDF5Data.h"



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

  Network* LoaderHDF5Data::loadNetwork( const std::string& networkFile,
                                      const std::string&  )
  {
    Network* _network = new Network( );

    if ( _h5Network == nullptr )
    {
      _h5Network = new H5Network( networkFile );
      _h5Network->load( );
    }
    _network->setDataType(THDF5);

    _network->setGids( _h5Network->getGIDs( ) );

    _network->setPositions( _h5Network->getComposedPositions( ) );

    SubsetEventManager subsetEventManager;
    auto subsetIts = _h5Network->getSubsets( );
    for ( simil::SubsetMapCIt it = subsetIts.first; it != subsetIts.second;
          ++it )
      subsetEventManager.addSubset( it->first, it->second );

    _network->setSubset( subsetEventManager );

    return _network;
  }

  SimulationData*
    LoaderHDF5Data::loadSimulationData( const std::string& networkFile,
                                        const std::string& activityFile )
  {
    SpikeData* simulationdata = new SpikeData( );

    if ( _h5Network == nullptr )
    {
      _h5Network = new H5Network( networkFile );
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

    simil::H5Spikes spikeReport( *_h5Network, activityFile );
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
