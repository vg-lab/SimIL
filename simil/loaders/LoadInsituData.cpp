/*
 * @file  LoadInsituData.h
 * @brief
 * @author Aaron Sújar <aaron.sujar@urjc.es>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 *          Do not distribute without further notice.
 */

#include <iostream>

#include "LoadInsituData.h"
#include "../SimulationData.h"

namespace simil
{
  // Cone _cone;
  LoadInsituData::LoadInsituData( )
    : LoadSimData( )
    //, _dataset( nullptr )
    , _simulationdata( nullptr )
    , _cone( nullptr )
    , waitForData( false )
  {
  }

  LoadInsituData::~LoadInsituData( )
  {
    waitForData = false;
    looper.join( );
    delete _cone;
  }

  /*DataSet* LoadInsituData::LoadNetwork( const std::string& filePath_,
                                        const std::string& target )
  {
    _dataset = new DataSet( filePath_, TInSitu, target );

    // Cone cone;
    // if (cone == nullptr)
    {
      if ( target.compare( "File" ) == 0 )
      {
        // cone::ConnectUsingConfigurationFile("path_to_config_file.json");
      }
      if ( target.compare( "ZeroMQ" ) == 0 )
      {
        // cone::ConnectViaZeroMQ(const std::string& address)
      }
      if ( target.compare( "Websocket" ) == 0 )
      {
        // cone::ConnectToWebsocketServer(const std::string& address,
        //                          unsigned int timeout_ms)
      }
      if ( target.compare( "Sharememory" ) == 0 )
      {
        // cone::ConnectViaSharedMemory(const std::string& shared_memory_name)
      }
    }

    // cone.SetNestMultimeterCallback(NetworkDataCB);

    return _dataset;
  }*/

  SimulationData*
    LoadInsituData::LoadSimulationData( const std::string& filePath_,
                                        const std::string& )
  {
    if ( _cone == nullptr )
    {
      if ( filePath_.empty( ) )
      {
        _cone =
          new cone::Cone( cone::ConnectUsingConfigurationFile( filePath_ ) );
      }
      else
      {
        _cone = new cone::Cone( cone::ConnectViaSharedMemory( ) );
        std::cout << "Config file not detected. "
                     "Connected through SharedMemory by default."
                  << std::endl;
      }
      auto m_cb = std::bind( &LoadInsituData::SpikeDetectorCB, this,
                             std::placeholders::_1 );
      _cone->SetSpikeDetectorCallback( m_cb );
    }

    if ( _simulationdata == nullptr )
      _simulationdata = new SimulationData( );

    looper = std::thread( &LoadInsituData::CBloop, this );

    return _simulationdata;
  } // namespace simil

  void LoadInsituData::CBloop( )
  {
    if ( _cone == nullptr )
    {
      std::cerr << "Cone not started";
      return;
    }
    waitForData = true;
    while ( waitForData )
    {
      _cone->Poll( 100 /* optional timeout*/ );
    }
  }

  void LoadInsituData::SpikeDetectorCB(
    const nesci::consumer::SpikeDetectorDataView& data )
  {
    Spikes _spikes = dynamic_cast< SpikeData* >( _simulationdata )->spikes( );
    auto timesteps = data.GetTimesteps( );
    auto neuron_ids = data.GetNeuronIds( );

    uint numOfElem = timesteps.number_of_elements( );

    float startTime = _simulationdata->startTime( );
    float endTime = _simulationdata->endTime( );

    for ( uint i = 0; i < numOfElem; ++i )
    {
      float timestamp = timesteps[ i ];
      if ( timestamp < startTime )
        _simulationdata->setStartTime( timestamp );
      if ( timestamp > endTime )
        _simulationdata->setEndTime( timestamp );
      _spikes.push_back( std::make_pair( timestamp, neuron_ids[ i ] ) );
    }

    // simil::StorageSparse *newStorage = new StorageSparse("Spikes",
    //                                                     tTYPE_UINT,
    //                                                     TSimSpikes);

    // newStorage->setSpikes(SpikeData.GetTimesteps(),
    //                      SpikeData.GetNeuronIds());

    // simulationdata->addStorage(newStorage);
  }
  /*void LoadInsituData::NetworkDataCB(
      const nesci::consumer::SetNestMultimeterDataView & NetData)
  {
      //dataset->setGids(NetData.GetNeuronIds());

      //dataset->setPositions(NetData->getComposedPositions());
  }
  void LoadInsituData::UnkwonDataCB(const conduit::Node &)
  {
  }*/

} // namespace simil
