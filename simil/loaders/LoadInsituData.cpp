/*
 * @file  LoadInsituData.h
 * @brief
 * @author Aaron Sújar <aaron.sujar@urjc.es>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 *          Do not distribute without further notice.
 */



#include "LoadInsituData.h"

namespace simil
{
  // Cone _cone;
  LoadInsituData::LoadInsituData( )
    : LoadSimData( )
    //, _dataset( nullptr )
    , _simulationdata( nullptr )
  {
  }

  LoadInsituData::~LoadInsituData( )
  {
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
                                        const std::string& target )
  {
    _simulationdata = new SimulationData( );

    if (filePath_.compare(target)==0)
    {

    }
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
    // cone.SetSpikeDetectorCallback(

    return _simulationdata;
  }

  /*void LoadInsituData::SpikeDetectorCB(
      const nesci::consumer::SpikeDetectorDataView & SpikeData)
  {
   //simil::StorageSparse *newStorage = new StorageSparse("Spikes",
      //                                                     tTYPE_UINT,
      //                                                     TSimSpikes);

      //newStorage->setSpikes(SpikeData.GetTimesteps(),
                              SpikeData.GetNeuronIds());

      //simulationdata->setStartTime();
      //simulationdata->setEndTime();

      //simulationdata->addStorage(newStorage);

  }
  void LoadInsituData::NetworkDataCB(
      const nesci::consumer::SetNestMultimeterDataView & NetData)
  {
      //dataset->setGids(NetData.GetNeuronIds());

      //dataset->setPositions(NetData->getComposedPositions());
  }
  void LoadInsituData::UnkwonDataCB(const conduit::Node &)
  {
  }*/

} // namespace simil


