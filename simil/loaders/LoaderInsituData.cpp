/*
 * @file  LoadInsituData.h
 * @brief
 * @author Aaron Sújar <aaron.sujar@urjc.es>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 *          Do not distribute without further notice.
 */

#include <iostream>

#include "LoaderInsituData.h"
#include "../SimulationData.h"

namespace simil
{
  // Cone _cone;
  LoaderInsituData::LoaderInsituData( )
    : LoaderSimData( )
    , _simulationdata( nullptr )
    , _network( nullptr )
    , _cone( nullptr )
    , _waitForData( false )
  {
  }

  LoaderInsituData::~LoaderInsituData( )
  {
    _waitForData = false;
    _looper.join( );
    delete _cone;
  }

  Network* LoaderInsituData::loadNetwork( const std::string& ,
                                          const std::string& target )
  {
    _network = new Network( );

    _network->setDataType( TCONE );

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

    return _network;
  }

  SimulationData*
    LoaderInsituData::loadSimulationData( const std::string& configFilePath,
                                          const std::string& )
  {
    if ( _cone == nullptr )
    {
      if ( !configFilePath.empty( ) )
      {
        _cone =
          new cone::Cone( cone::ConnectUsingConfigurationFile( configFilePath ) );
      }
      else
      {
        _cone = new cone::Cone( cone::ConnectViaSharedMemory( ) );
        std::cout << "Config file not detected. "
                     "Connected through SharedMemory by default."
                  << std::endl;
      }
      auto m_spikecb = std::bind( &LoaderInsituData::SpikeDetectorCB, this,
                                  std::placeholders::_1 );
      _cone->SetSpikeDetectorCallback( m_spikecb );

      auto m_networkcb = std::bind( &LoaderInsituData::NetworkDataCB, this,
                                    std::placeholders::_1 );
      _cone->SetNestMultimeterCallback( m_networkcb );
    }

    if ( _simulationdata == nullptr )
      _simulationdata = new SpikeData( );

    _looper = std::thread( &LoaderInsituData::CBloop, this );

    return _simulationdata;
  } // namespace simil

  void LoaderInsituData::CBloop( )
  {
    if ( _cone == nullptr )
    {
      std::cerr << "Cone not started";
      return;
    }
    _waitForData = true;
    while ( _waitForData )
    {
      _cone->Poll( 1 /* optional timeout*/ );
    }
  }

  void LoaderInsituData::SpikeDetectorCB(
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
      if ( timestamp >= endTime )
      {
        _spikes->setEndTime( timestamp );
        _spikes->addSpike( timestamp, neuron_ids[ i ] );
      }
    }

    // simil::StorageSparse *newStorage = new StorageSparse("Spikes",
    //                                                     tTYPE_UINT,
    //                                                     TSimSpikes);

    // newStorage->setSpikes(SpikeData.GetTimesteps(),
    //                      SpikeData.GetNeuronIds());

    // simulationdata->addStorage(newStorage);
  }
  void LoaderInsituData::NetworkDataCB(
    const nesci::consumer::NestMultimeterDataView& NetData )
  {
    auto gids = NetData.GetNeuronIds( );
    auto numGids = gids.number_of_elements( );

    if ( numGids < 1 )
      return;

    const TGIDSet& spikes = _simulationdata->gids( );
    if ( spikes.count( gids[ 0 ] ) > 0 ) // It's old data
      return;

    auto pos = NetData.GetFloatingPointAttributeValues( "Positions" );
    auto numPos = pos.number_of_elements( );

    if ( numPos > 0 )
    {
      for ( unsigned int i = 0; i < numPos; i++ )
      {
        _simulationdata->setPosition(
          vmml::Vector3f( pos[ i ], pos[ i ], pos[ i ] ) );
      }
      for ( unsigned int i = 0; i < numGids; i++ )
      {
        _simulationdata->setGid( gids[ i ] );
      }
    }
    else
    {
      unsigned int width = std::sqrt( numGids );
      for ( unsigned int i = 0; i < numGids; i++ )
      {
        _simulationdata->setGid( gids[ i ] );
        _simulationdata->setPosition(
          vmml::Vector3f( i / width, i % width, 0 ) );
      }
    }
  }

} // namespace simil
