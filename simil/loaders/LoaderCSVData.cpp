/*
 * @file  LoadHDF5Data.h
 * @brief
 * @author Aaron Sújar <aaron.sujar@urjc.es>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 *          Do not distribute without further notice.
 */

#include "LoaderCSVData.h"

//#include "../storage/StorageSparse.h"

namespace simil
{
  LoaderCSVData::LoaderCSVData( )
    : LoaderSimData( )
    , _csvNetwork( nullptr )
    , _csvActivity( nullptr )
  {
  }

  LoaderCSVData::~LoaderCSVData( )
  {
    if ( _csvNetwork != nullptr )
      delete _csvNetwork;

    if ( _csvActivity != nullptr )
      delete _csvActivity;
  }

  /*DataSet* LoadCSVData::LoadNetwork( const std::string& filePath_,
                                      const std::string& target )
  {
    DataSet* dataset = new DataSet( filePath_, THDF5, target );

    if ( _csvNetwork != nullptr )
    {
      _csvNetwork = new CSVNetwork( filePath_ );
      _csvNetwork->load( );
    }

    dataset->setGids( _csvNetwork->getGIDs( ) );

    dataset->setPositions( _csvNetwork->getComposedPositions( ) );

    return dataset;
  }*/

  SimulationData*
    LoaderCSVData::loadSimulationData( const std::string& filePath_,
                                       const std::string& target )
  {
    SpikeData* simulationdata = new SpikeData( );

    if ( _csvNetwork == nullptr )
    {
      _csvNetwork = new CSVNetwork( filePath_ );
      _csvNetwork->load( );
    }

    simulationdata->setGids( _csvNetwork->getGIDs( ) );

    simulationdata->setPositions( _csvNetwork->getComposedPositions( ) );

    if ( _csvActivity == nullptr )
    {
      _csvActivity = new CSVSpikes( *_csvNetwork, target, ',', false );
      _csvActivity->load( );
    }

    CSVSpikes* _csvSpikes = dynamic_cast< CSVSpikes* >( _csvActivity );

    /*simil::StorageSparse* newStorage =
      new StorageSparse( "Spikes", tTYPE_UINT, TSimSpikes );*/

    simulationdata->setSimulationType( TSimSpikes );
    simulationdata->setSpikes( _csvSpikes->spikes( ) );
    simulationdata->setStartTime( _csvSpikes->startTime( ) );
    simulationdata->setEndTime( _csvSpikes->endTime( ) );

    // simulationdata->addStorage( newStorage );

    return simulationdata;
  }

} // namespace simil
