/*
 * Copyright (c) 2015-2020 VG-Lab/URJC.
 *
 * Authors: Aaron Sujar <aaron.sujar@urjc.es>
 *
 * This file is part of SimIL <https://github.com/vg-lab/SimIL>
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

// Project
#include "LoaderRestData.h"

#include "HTTP/SyncClient.h"

// C++
#include <iostream>
#include <memory>

// JsonCpp
#include "jsoncpp/json/json.h"
#include "simil/Network.h"
#include "simil/SimulationData.h"
#include "simil/SpikeData.h"

constexpr uint64_t RANGE_LIMIT = std::numeric_limits< uint32_t >::max( );

namespace simil
{
  const std::string LoaderRestData::ARBOR_PREFIX = "/arbor";
  const std::string LoaderRestData::NEST_PREFIX = "/nest";

  LoaderRestData::LoaderRestData( )
    : LoaderSimData( )
    , _forceStop{ false }
    , _spikesRead{ 0 }
  { }

  LoaderRestData::~LoaderRestData( )
  {
  }

  std::unique_ptr< SimulationData >
  LoaderRestData::loadSimulationData( const std::string& url ,
                                      const std::string& port )
  {
    auto serverUrl = m_config.url;
    unsigned int serverPort = m_config.port;

    if ( !url.empty( ))
      serverUrl = url;

    if ( !port.empty( ))
      serverPort = stoi( port );

    m_config.url = serverUrl;
    m_config.port = serverPort;

    auto data = new SpikeData( );
    loopSpikes( data , serverUrl , restAPIPrefix( ) , serverPort );

    return std::unique_ptr< SimulationData >( data );
  }

  std::unique_ptr< Network >
  LoaderRestData::loadNetwork( const std::string& url ,
                               const std::string& port )
  {
    auto serverUrl = m_config.url;
    unsigned int serverPort = m_config.port;

    if ( !url.empty( ))
      serverUrl = url;

    if ( !port.empty( ))
      serverPort = stoi( port );

    m_config.url = serverUrl;
    m_config.port = serverPort;

    auto network = new Network( );

    loopNetwork( network , serverUrl , restAPIPrefix( ) , serverPort );
    return std::unique_ptr< Network >( network );
  }

  LoaderRestData::RESTResult
  LoaderRestData::callbackSpikes( SpikeData* spikes , std::istream& data ) const
  {
    if ( data.eof( ) || data.fail( )) return { RESTResultType::NODATA , false };
    Json::Value root;

    try
    {
      data >> root;
    } catch ( const std::exception& e )
    {
      auto message = std::string( e.what( ));
      std::replace( message.begin( ) , message.end( ) , '\n' , ' ' );
      std::cerr << "REST - NODES ERROR: callbackSpikes Exception JSON PARSER ("
                << message << ")" << std::endl;
      std::cerr << "JSON ROOT is: " << root << std::endl;
      return { RESTResultType::EXCEPTION , false };
    }

    if ( root.empty( )) return { RESTResultType::NODATA , false };

    // Fetch data from Json.
    auto nodeIds = root[ "nodeIds" ];
    auto times = root[ "simulationTimes" ];
    auto last = root[ "lastFrame" ];

    // No new spikes. Skip!
    if ( nodeIds.empty( ) || times.empty( ))
      return { RESTResultType::NODATA , last.isBool( ) && last.asBool( ) };

    TSpikes vecSpikes;
    vecSpikes.reserve( m_config.spikesSize );

    float startTime = spikes->startTime( );
    float endTime = spikes->endTime( );
    uint32_t rangeErrors = 0;

    for ( uint32_t idx = 0; idx < nodeIds.size( ); ++idx )
    {
      auto nodeId = nodeIds[ idx ].asUInt64( );
      if ( nodeId > RANGE_LIMIT )
      {
        ++rangeErrors;
        continue;
      }

      auto time = times[ idx ].asFloat( );
      startTime = std::min( time , startTime );
      endTime = std::max( time , endTime );

      vecSpikes.emplace_back( time , static_cast<uint32_t>(nodeId));
    }

    if ( !vecSpikes.empty( ))
    {
      spikes->addSpikes( vecSpikes );
      spikes->setStartTime( startTime );
      spikes->setEndTime( endTime );
    }

    if ( rangeErrors > 0 )
    {
      std::cerr << "REST - NODES ERROR: Node ids outside of uin32_t range: "
                << rangeErrors << std::endl;
    }

    // There are always new spikes when we reach this code.
    return { RESTResultType::NEWDATA , last.isBool( ) && last.asBool( ) };
  }

  LoaderRestData::RESTResult
  LoaderRestData::callbackNodeProperties( Network* network ,
                                          std::istream& contentdata )
  {
    if ( contentdata.eof( ) || contentdata.fail( ))
      return { RESTResultType::NODATA , false };

    SubsetMap populationMap;
    TGIDVect gids;
    TPosVect positions;
    unsigned long rangeErrors = 0;
    Json::Value root;

    try
    {
      contentdata >> root;

      if ( root.empty( ) || root.isNull( ) ||
           !root.isArray( ))
        return { RESTResultType::NODATA , false };

      auto& oldgids = network->gids( );

      for ( unsigned int idx = 0; idx < root.size( ); ++idx )
      {
        auto props = root[ idx ];
        if ( props.isNull( )) continue;

        const auto status = props["nodeStatus"];
        const auto element_type = status[ "element_type" ].asString();
        if(element_type.compare("neuron") != 0) continue;

        const auto gid = props[ "nodeId" ].asUInt64( );
        if ( gid > RANGE_LIMIT )
        {
          ++rangeErrors;
          continue;
        }

        if ( oldgids.find( gid ) != oldgids.end( )) continue;

        const auto gid32 = static_cast<uint32_t>(gid);

        gids.push_back( gid32 );

        // according to the model the collectionId is a number.
        const auto groupId = props[ "nodeCollectionId" ].asUInt64( );
        if ( groupId > RANGE_LIMIT )
        {
          ++rangeErrors;
          continue;
        }
        const auto groupId32 = static_cast<uint32_t>(groupId);

        populationMap[ std::to_string( groupId32 ) ].push_back( gid32 );

        const auto position = props[ "position" ];

        vmml::Vector3f positionVec{ 0. , 0. , 0. };
        for ( unsigned int i = 0; i < position.size( ); ++i )
          positionVec[ i ] = position[ i ].asFloat( );

        positions.push_back( positionVec );
      }
    }
    catch ( const std::exception& e )
    {
      auto message = std::string( e.what( ));
      std::replace( message.begin( ) , message.end( ) , '\n' , ' ' );
      std::cerr << "REST - NODES ERROR: callbackNodes Exception JSON PARSER ("
                << message << ")" << std::endl;
      std::cerr << "JSON ROOT is: " << root << std::endl;
      return { RESTResultType::EXCEPTION , false };
    }

    if ( !gids.empty( ))
    {
      network->setNeurons( gids , positions );
    }

    if ( gids.size( ) != positions.size( ))
    {
      std::cerr
        << "REST - NODES ERROR: callbackNodeProperties gids and positions mismatch ("
        << gids.size( ) << " != " << positions.size( ) << ")" << std::endl;
    }

    if ( !populationMap.empty( ))
    {
      auto addSubset = [ network ](
        const std::pair< std::string , GIDVec >& item )
      {
        network->subsetsEvents( )->addSubset(
          std::string( "Subset " ) + item.first , item.second );
      };
      std::for_each( populationMap.cbegin( ) , populationMap.cend( ) ,
                     addSubset );
    }

    if ( rangeErrors > 0 )
    {
      std::cerr
        << "REST - NODES ERROR: Node or group ids outside of uin32_t range ("
        << rangeErrors << ")" << std::endl;
    }

    auto type = ( !gids.empty( ) || !populationMap.empty( ))
                ? RESTResultType::NEWDATA
                : RESTResultType::NODATA;
    return { type , false };
  }

  void LoaderRestData::loopSpikes( SpikeData* data ,
                                   const std::string& url ,
                                   const std::string& prefix ,
                                   const unsigned int port )
  {
    while ( !_forceStop )
    {
      const auto result = getSpikes( data , url , prefix , port );
      if ( result.stopThread ) break;
      switch ( result.type )
      {
        case RESTResultType::NOTCONNECTED:
        case RESTResultType::EXCEPTION:
          std::this_thread::sleep_for(
            std::chrono::milliseconds( m_config.failTime ));
          break;
        case RESTResultType::NODATA:
          std::this_thread::sleep_for(
            std::chrono::milliseconds( m_config.waitTime ));
          break;
        case RESTResultType::NEWDATA:
          break;
      }
    }

  }

  void LoaderRestData::loopNetwork( Network* network ,
                                    const std::string& url ,
                                    const std::string& prefix ,
                                    const unsigned int port )
  {
    // NOTE: aborts after getting node positions, as NEST doesn't add new nodes later.
    while ( !_forceStop && network->gidsSize( ) < 2 )
    {
      const auto result = getNodeProperties( network , url , prefix , port );
      if ( result.stopThread ) break;
      switch ( result.type )
      {
        case RESTResultType::NOTCONNECTED:
        case RESTResultType::EXCEPTION:
          std::this_thread::sleep_for(
            std::chrono::milliseconds( m_config.failTime ));
          break;
        case RESTResultType::NODATA:
          std::this_thread::sleep_for(
            std::chrono::milliseconds( m_config.waitTime ));
          break;
        case RESTResultType::NEWDATA:
          break;
      }
    }
  }

  LoaderRestData::RESTResult
  LoaderRestData::getNodeProperties( Network* network ,
                                     const std::string& url ,
                                     const std::string& prefix ,
                                     const unsigned int port )
  {
    HTTPSyncClient client;

    client.set_host( url );
    client.set_uri( prefix + "/nodes/" );
    client.set_port( port );

    const auto answer = client.execute( );

    if ( answer == boost::system::errc::success ) // Success
    {
      return callbackNodeProperties( network , client.get_response( ));
    }

    std::cerr << "REST - NODES ERROR: " << client.get_status_message( )
              << std::endl;

    return { RESTResultType::NOTCONNECTED , false };
  }

  LoaderRestData::RESTResult
  LoaderRestData::getSpikes( SpikeData* spikes ,
                             const std::string& url ,
                             const std::string& prefix ,
                             const unsigned int port )
  {
    // Let's fetch the spikes from api.xxx/prefix/spikes/
    HTTPSyncClient client;
    std::string uri( prefix + "/spikes/?" );
    if ( _spikesRead > 0 )
    {
      uri.append( "skip=" );
      uri.append( std::to_string( _spikesRead ));
      uri.append( "&" );
    }
    uri.append( "top=" );
    uri.append( std::to_string( m_config.spikesSize ));

    client.set_host( url );
    client.set_uri( uri );
    client.set_port( port );

    const auto answer = client.execute( );

    if ( answer == boost::system::errc::success )
    {
      const auto result = callbackSpikes( spikes , client.get_response( ));
      if ( result.type == RESTResultType::NEWDATA )
      {
        _spikesRead = spikes->spikes( ).size( );
      }

      return result;
    }

    std::cerr << "REST - SPIKES ERROR: " << client.get_status_message( )
              << std::endl;

    return { RESTResultType::NOTCONNECTED , false };
  }

  struct LoaderRestData::Version
  LoaderRestData::getVersion( const std::string url , const unsigned int port )
  {
    struct Version result;
    HTTPSyncClient client;

    client.set_host( url );
    client.set_uri( "/version/" );
    client.set_port( port );
    const auto answer = client.execute( );

    if ( answer == boost::system::errc::success ) // Success
    {
      Json::Value root;

      try
      {
        client.get_response( ) >> root;
        result.api = root[ "api" ].asString( );
        result.insite = root[ "insite" ].asString( );
      }
      catch ( ... )
      {
      }
    }

    return result;
  }

  std::string LoaderRestData::restAPIPrefix( ) const
  {
    return m_config.api == Rest_API::NEST ? NEST_PREFIX : ARBOR_PREFIX;
  }

  void LoaderRestData::setConfiguration( const Configuration& config )
  {
    m_config = config;
  }

  LoaderRestData::Configuration LoaderRestData::getConfiguration( ) const
  {
    return m_config;
  }

} // namespace simil
