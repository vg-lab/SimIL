/*
 * Copyright (c) 2015-2020 GMRV/URJC.
 *
 * Authors: Aaron Sujar <aaron.sujar@urjc.es>
 *
 * This file is part of SimIL <https://github.com/gmrvvis/SimIL>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include "LoaderRestData.h"
#include "../SimulationData.h"

#include "HTTP/SyncClient.h"

#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace simil
{
  LoaderRestData::LoaderRestData( )
  : LoaderSimData( )
  , _instance( nullptr )
  , _simulationdata( nullptr )
  , _network( nullptr )
  , _waitForData( false )
  , _host( "localhost" )
  , _port( 8080 )
  , _deltaTime( 0.1f )
  , _dataOffset( 500 )
  , _spikesRead( 0 )
  { }

  LoaderRestData::~LoaderRestData( )
  {
    _waitForData = false;
    _looperSpikes.join( );
    _looperNetwork.join( );
  }

  SimulationData*
    LoaderRestData::loadSimulationData( const std::string& hostURL,
                                        const std::string& port )
  {
    _host = hostURL;
    if ( port.empty( ) )
      _port = 8080;
    else
      _port = static_cast< unsigned int >( std::stoi( port ) );

    if ( _simulationdata == nullptr )
      _simulationdata = new SpikeData( );

    _waitForData = true;
    _looperSpikes = std::thread( &LoaderRestData::loopSpikes, this );

    return _simulationdata;
  } // namespace simil

  Network* LoaderRestData::loadNetwork( const std::string& hostURL,
                                        const std::string& port )
  {
    _host = hostURL;
    if ( port.empty( ) )
      _port = 8080;
    else
      _port = static_cast< unsigned int >( std::stoi( port ) );

    if ( _network == nullptr )
      _network = new Network( );

    _waitForData = true;
    _looperNetwork = std::thread( &LoaderRestData::loopNetwork, this );

    return _network;
  }

  void LoaderRestData::network( Network* network )
  {
    _network = network;
  }

  void LoaderRestData::deltaTime( float deltaTime )
  {
    _deltaTime = deltaTime;
  }

  float LoaderRestData::deltaTime( )
  {
    return _deltaTime;
  }

  void LoaderRestData::dataOffset( unsigned int offset )
  {
    _dataOffset = offset;
  }
  unsigned int LoaderRestData::dataOffeset( )
  {
    return _dataOffset;
  }

  int LoaderRestData::callbackSpikes( std::istream& contentdata )
  {
    SpikeData* _spikes = dynamic_cast< SpikeData* >( _simulationdata );
    TSpikes vecSpikes;
    float startTime;
    float endTime;

    boost::property_tree::ptree propertytree;
    try
    {
      boost::property_tree::read_json( contentdata, propertytree );

      startTime = _spikes->startTime( );
      endTime = _spikes->endTime( );

      auto it_gids = propertytree.get_child( "gids" ).begin( );
      auto it_times = propertytree.get_child( "simulation_times" ).begin( );
      auto gids_end = propertytree.get_child( "gids" ).end( );

      vecSpikes.reserve( _dataOffset );

      for ( ; it_gids != gids_end; ++it_gids, ++it_times )
      {
        float timestamp = it_times->second.get_value< float >( ) * _deltaTime;
        if ( timestamp < startTime )
          startTime = timestamp;
        if ( timestamp >= endTime )
        {
          endTime = timestamp;
          vecSpikes.push_back( std::make_pair(
            timestamp, it_gids->second.get_value< unsigned int >( ) ) );
        }
      }
    }
    catch ( std::exception& e )
    {
      std::cerr << "Spikes Exception JSON PARSER:  " << e.what( ) << "\n";
      return EXCEPTION;
    }

    if ( vecSpikes.size( ) > 0 )
    {
      _spikes->addSpikes( vecSpikes );
      _spikes->setStartTime( startTime );
      _spikes->setEndTime( endTime );
      return NEWDATA;
    }

    return NODATA;
  }

  int LoaderRestData::callbackGids( std::istream& contentdata )
  {
    TGIDSet gidSet;

    boost::property_tree::ptree propertytree;
    try
    {
      boost::property_tree::read_json( contentdata, propertytree );
      auto it_gids = propertytree.begin( );
      auto gids_end = propertytree.end( );

      for ( ; it_gids != gids_end; ++it_gids )
      {
        float number = it_gids->second.get_value< float >( );
        gidSet.insert( static_cast< unsigned int >( number ) );
      }
    }
    catch ( std::exception& e )
    {
      std::cerr << "Gids Exception JSON PARSER:  " << e.what( ) << "\n";
      return EXCEPTION;
    }

    if ( gidSet.size( ) > 0 )
    {
      _network->setGids( gidSet, true );
      return NEWDATA;
    }
    return NODATA;
  }

  int LoaderRestData::callbackPopulations( std::istream& contentdata )
  {
    boost::property_tree::ptree propertytree;
    try
    {
      boost::property_tree::read_json( contentdata, propertytree );
    }
    catch ( std::exception& e )
    {
      std::cerr << "Exception JSON PARSER:  " << e.what( ) << "\n";
      std::cerr << contentdata.rdbuf( );
      return EXCEPTION;
    }
    return NODATA;
  }

  int LoaderRestData::callbackNProperties( std::istream& contentdata )
  {
    SubsetMap populationMap;
    TGIDVect gids;
    TPosVect positions;

    boost::property_tree::ptree propertytree;
    try
    {
      boost::property_tree::read_json( contentdata, propertytree );

      auto it_neuron = propertytree.begin( );
      auto it_end = propertytree.end( );

      float pos[ 3 ]{0, 0, 0};

      for ( ; it_neuron != it_end; ++it_neuron )
      {
        unsigned int gid =
          it_neuron->second.get_child( "gid" ).get_value< unsigned int >( );
        gids.push_back( gid );

        auto properties = it_neuron->second.get_child( "properties" );

        std::string group =
          properties.get_child( "population" ).get_value< std::string >( );
        populationMap[ group ].push_back( gid );

        auto position = properties.get_child( "position" );

        int i = 0;
        for ( auto it = position.begin( ); it != position.end( ); ++it )
        {
          pos[ i ] = it->second.get_value< float >( );
          ++i;
        }
        vmml::Vector3f x( pos[ 0 ], pos[ 1 ], pos[ 2 ] );
        positions.push_back( x );
      }
    }
    catch ( std::exception& e )
    {
      std::cerr << "NProp Exception JSON PARSER:  " << e.what( ) << "\n";

      return EXCEPTION;
    }

    if ( gids.size( ) > 0 )
    {
      _network->setNeurons( gids, positions );
    }

    if ( populationMap.size( ) > 0 )
    {
      for ( auto it = populationMap.begin( ); it != populationMap.end( ); ++it )
      {
        _network->subsetsEvents( )->addSubset( it->first, it->second );
      }
      return NEWDATA;
    }

    return ( gids.size( ) > 0 || populationMap.size( ) > 0 ) ? NEWDATA : NODATA;
  }

  int LoaderRestData::callbackTime( std::istream& contentdata )
  {
    boost::property_tree::ptree propertytree;
    try
    {
      boost::property_tree::read_json( contentdata, propertytree );
    }
    catch ( std::exception& e )
    {
      std::cerr << "Exception JSON PARSER:  " << e.what( ) << "\n";
      std::cerr << contentdata.rdbuf( );
      return EXCEPTION;
    }
    return NODATA;
  }

  void LoaderRestData::loopSpikes( )
  {
    int result = NOTCONNECT;
    while ( _waitForData )
    {
      result = GETSpikes( );

      switch ( result )
      {
        case NOTCONNECT:
          std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ) );
          break;
        case EXCEPTION:
          std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
          break;
        case NODATA:
          std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ) );
          break;
        case NEWDATA:
          std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
          break;
      }
    }
  }
  void LoaderRestData::loopNetwork( )
  {
    int result = NOTCONNECT;
    while ( _waitForData )
    {
      GETTimeInfo( );
      // GETGids( );
      result = GETNeuronProperties( );
      // GETPopulations( );

      switch ( result )
      {
        case NOTCONNECT:
          std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ) );
          break;
        case EXCEPTION:
          std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
          break;
        case NODATA:
          std::this_thread::sleep_for( std::chrono::milliseconds( 30000 ) );
          break;
        case NEWDATA:
          std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ) );
          break;
      }
    }
  }

  int LoaderRestData::GETTimeInfo( )
  {
    HTTPSyncClient client;

    client.set_host( _host );
    client.set_uri( "/simulation_time_info" );
    client.set_port( _port );

    int result = client.execute( );

    if ( result == boost::system::errc::success ) // Success
    {
      result = callbackTime( client.get_response( ) );
    }
    else
      result = NOTCONNECT;

    return result;
  }
  int LoaderRestData::GETGids( )
  {
    HTTPSyncClient client;

    client.set_host( _host );
    client.set_uri( "/gids" );
    client.set_port( _port );

    int result = client.execute( );

    if ( result == boost::system::errc::success ) // Success
    {
      result = callbackGids( client.get_response( ) );
    }
    else
      result = NOTCONNECT;

    return result;
  }
  int LoaderRestData::GETNeuronProperties( )
  {
    HTTPSyncClient client;

    client.set_host( _host );
    client.set_uri( "/neuron_properties" );
    client.set_port( _port );

    int result = client.execute( );

    if ( result == boost::system::errc::success ) // Success
    {
      result = callbackNProperties( client.get_response( ) );
    }
    else
      result = NOTCONNECT;
    return result;
  }
  int LoaderRestData::GETPopulations( )
  {
    HTTPSyncClient client;

    client.set_host( _host );
    client.set_uri( "/populations" );
    client.set_port( _port );

    int result = client.execute( );

    if ( result == boost::system::errc::success ) // Success
    {
      result = callbackPopulations( client.get_response( ) );
    }
    else
      result = NOTCONNECT;

    return result;
  }

  int LoaderRestData::GETSpikes( )
  {
    HTTPSyncClient client;

    std::string uri( "/spikes?limit=" );
    uri.append( std::to_string( _dataOffset ) );
    uri.append( "&offset=" );
    uri.append( std::to_string( _spikesRead ) );

    client.set_host( _host );
    client.set_uri( uri );
    client.set_port( _port );

    int result = client.execute( );

    if ( result == boost::system::errc::success ) // Success
    {
      result = callbackSpikes( client.get_response( ) );
      if ( result == NEWDATA )
        _spikesRead += _dataOffset;
    }
    else
      result = NOTCONNECT;

    return result;
  }

} // namespace simil
