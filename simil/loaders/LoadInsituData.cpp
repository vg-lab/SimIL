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
    , _waitForData( false )
  {
  }

  LoadInsituData::~LoadInsituData( )
  {
    _waitForData = false;
    _looper.join( );
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
      if ( !filePath_.empty( ) )
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
      auto m_spikecb = std::bind( &LoadInsituData::SpikeDetectorCB, this,
                                  std::placeholders::_1 );
      _cone->SetSpikeDetectorCallback( m_spikecb );

      auto m_networkcb = std::bind( &LoadInsituData::NetworkDataCB, this,
                                    std::placeholders::_1 );
      _cone->SetNestMultimeterCallback( m_networkcb );
    }

    if ( _simulationdata == nullptr )
      _simulationdata = new SpikeData( );

    _looper = std::thread( &LoadInsituData::CBloop, this );

    return _simulationdata;
  } // namespace simil

  void LoadInsituData::CBloop( )
  {
    if ( _cone == nullptr )
    {
      std::cerr << "Cone not started";
      return;
    }
    _waitForData = true;
    while ( _waitForData )
    {
      _cone->Poll( 100 /* optional timeout*/ );
    }
  }

  void LoadInsituData::SpikeDetectorCB(
    const nesci::consumer::SpikeDetectorDataView& data )
  {
    SpikeData* _spikes = dynamic_cast< SpikeData* >( _simulationdata );
    auto timesteps = data.GetTimesteps( );
    auto neuron_ids = data.GetNeuronIds( );

    unsigned int numOfElem = timesteps.number_of_elements( );

    float startTime = _spikes->startTime( );
    float endTime = _spikes->endTime( );

    for ( unsigned int i = 0; i < numOfElem; ++i )
    {
      float timestamp = timesteps[ i ];
      if ( timestamp < startTime )
        _spikes->setStartTime( timestamp );
      if ( timestamp > endTime )
        _spikes->setEndTime( timestamp );
      _spikes->addSpike( timestamp, neuron_ids[ i ] );
    }

    // simil::StorageSparse *newStorage = new StorageSparse("Spikes",
    //                                                     tTYPE_UINT,
    //                                                     TSimSpikes);

    // newStorage->setSpikes(SpikeData.GetTimesteps(),
    //                      SpikeData.GetNeuronIds());

    // simulationdata->addStorage(newStorage);
  }
  void LoadInsituData::NetworkDataCB(
    const nesci::consumer::NestMultimeterDataView& NetData )
  {
    auto gids = NetData.GetNeuronIds( );
    auto numGids =  gids.number_of_elements( );
    for ( unsigned int i = 0; i <numGids; i++ )
    {
      _simulationdata->setGid( gids[ i ] );
    }

    auto pos = NetData.GetFloatingPointAttributeValues( "Positions" );
    auto numPos = pos.number_of_elements( );
    for ( unsigned int i = 0; i < numPos; i++ )
    {
      _simulationdata->setPosition(
        vmml::Vector3f( pos[ i ], pos[ i ], pos[ i ] ) );
    }
  }


} // namespace simil
