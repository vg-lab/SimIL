/*
 * @file  LoadHDF5Data.h
 * @brief
 * @author Aaron Sújar <aaron.sujar@urjc.es>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 *          Do not distribute without further notice.
 */

#include "LoaderCSVData.h"



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

  Network* LoaderCSVData::loadNetwork( const std::string& filePath_,
                                      const std::string&  )
  {
    Network* _network = new Network( );

    if ( _csvNetwork == nullptr )
    {
      _csvNetwork = new CSVNetwork( filePath_ );
      _csvNetwork->load( );

    }
    _network->setDataType(TCSV);

    _network->setGids( _csvNetwork->getGIDs( ) );

    _network->setPositions( _csvNetwork->getComposedPositions( ) );

    return _network;
  }

  SimulationData*
    LoaderCSVData::loadSimulationData( const std::string& ,
                                       const std::string& activityFile )
  {
    SpikeData* simulationdata = new SpikeData( );


    if ( _csvActivity == nullptr )
    {
      _csvActivity = new CSVSpikes( *_csvNetwork, activityFile, ',', false );
      _csvActivity->load( );
    }

    CSVSpikes* _csvSpikes = dynamic_cast< CSVSpikes* >( _csvActivity );

    simulationdata->setSimulationType( TSimSpikes );
    simulationdata->setSpikes( _csvSpikes->spikes( ) );
    simulationdata->setStartTime( _csvSpikes->startTime( ) );
    simulationdata->setEndTime( _csvSpikes->endTime( ) );

    return simulationdata;
  }

} // namespace simil
