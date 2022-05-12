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
#include "../SimulationData.h"

#include "HTTP/SyncClient.h"

// C++
#include <iostream>

// JsonCpp
#include "jsoncpp/json/json.h"

constexpr uint64_t RANGE_LIMIT = std::numeric_limits<uint32_t>::max();

namespace simil
{
  const std::string LoaderRestData::ARBOR_PREFIX   = "/arbor";
  const std::string LoaderRestData::NEST_PREFIX    = "/nest";

  LoaderRestData::LoaderRestData( )
  : LoaderSimData( )
  , _simulationdata{ nullptr }
  , _network{ nullptr }
  , _waitForData{ false }
  , _deltaTime{ 0.1f }
  , _spikesRead{ 0 }
  { }

  LoaderRestData::~LoaderRestData( )
  {
    _waitForData = false;
    _looperSpikes.join( );
    _looperNetwork.join( );
  }

  SimulationData*
    LoaderRestData::loadSimulationData( const std::string& url,
                                        const std::string& port )
  {
    auto serverUrl  = m_config.url;
    unsigned int serverPort = m_config.port;

    if ( !url.empty() )
      serverUrl = url;

    if(!port.empty())
      serverPort = stoi(port);

    m_config.url = serverUrl;
    m_config.port = serverPort;

    _simulationdata = new SpikeData( );

    _waitForData = true;
    _looperSpikes = std::thread( &LoaderRestData::loopSpikes, this,
                                 serverUrl, restAPIPrefix(), serverPort );

    return _simulationdata;
  }

  Network* LoaderRestData::loadNetwork( const std::string& url,
                                        const std::string& port )
  {
    auto serverUrl  = m_config.url;
    unsigned int serverPort = m_config.port;

    if ( !url.empty() )
      serverUrl = url;

    if(!port.empty())
      serverPort = stoi(port);

    m_config.url = serverUrl;
    m_config.port = serverPort;

    _network = new Network( );

    _waitForData = true;
    _looperNetwork = std::thread( &LoaderRestData::loopNetwork, this,
                                  serverUrl, restAPIPrefix(), serverPort );

    return _network;
  }

  void LoaderRestData::deltaTime( float deltaTime )
  {
    _deltaTime = deltaTime;
  }

  float LoaderRestData::deltaTime( )
  {
    return _deltaTime;
  }

  LoaderRestData::RESTResult LoaderRestData::callbackSpikes( std::istream& contentdata )
  {
    if(contentdata.eof() || contentdata.fail() || !_waitForData) return RESTResult::NODATA;

    TSpikes vecSpikes;
    auto _spikes = dynamic_cast< SpikeData* >( _simulationdata );
    float startTime = 0, endTime = 0;
    unsigned long rangeErrors = 0;
    Json::Value root;

    try
    {
      contentdata >> root;
      if(!_waitForData || root.empty()) return RESTResult::NODATA;

      const Json::Value nodeIds = root["nodeIds"];
      const Json::Value times = root["simulationTimes"];

      if(!_waitForData || nodeIds.empty() || times.empty()) return RESTResult::NODATA;

      startTime = _spikes->startTime();
      endTime = _spikes->endTime();

      vecSpikes.reserve(m_config.spikesSize);

      for (unsigned int idx = 0; idx < nodeIds.size(); ++idx)
      {
        const auto nodeId = nodeIds[idx].asUInt64();

        if (nodeId > RANGE_LIMIT)
        {
          ++rangeErrors;
          continue;
        }

        const auto timestamp = static_cast<float>(times[idx].asDouble());

        startTime = std::min(timestamp, startTime);
        endTime = std::max(timestamp, endTime);

        if (timestamp == endTime)
        {
          vecSpikes.push_back(
              std::make_pair(timestamp, static_cast<uint32_t>(nodeId)));
        }
      }
    }
    catch (const std::exception &e)
    {
      auto message = std::string(e.what());
      std::replace(message.begin(), message.end(), '\n', ' ');
      std::cerr << "REST - NODES ERROR: callbackSpikes Exception JSON PARSER (" << message << ")" << std::endl;
      std::cerr << "JSON ROOT is: " << root << std::endl;
      return RESTResult::EXCEPTION;
    }

    if(!_waitForData) return RESTResult::NODATA;

    if ( !vecSpikes.empty() )
    {
      _spikes->addSpikes( vecSpikes );
      _spikes->setStartTime( startTime );
      _spikes->setEndTime( endTime );
    }

    if(rangeErrors > 0)
    {
      std::cerr << "REST - NODES ERROR: Node ids outside of uin32_t range: " << rangeErrors << std::endl;
    }

    return vecSpikes.empty() ? RESTResult::NODATA : RESTResult::NEWDATA;
  }

  LoaderRestData::RESTResult LoaderRestData::callbackNodeProperties( std::istream& contentdata )
  {
    if(contentdata.eof() || contentdata.fail() || !_waitForData) return RESTResult::NODATA;

    SubsetMap populationMap;
    TGIDVect gids;
    TPosVect positions;
    unsigned long rangeErrors = 0;
    Json::Value root;

    try
    {
      contentdata >> root;

      if(!_waitForData || root.empty() || root.isNull() || !root.isArray()) return RESTResult::NODATA;

      auto &oldgids = _network->gids();

      for(unsigned int idx = 0; idx < root.size(); ++idx)
      {
        auto props = root[idx];
        if(props.isNull()) continue;

        const auto gid = props["nodeId"].asUInt64();
        if (gid > RANGE_LIMIT)
        {
          ++rangeErrors;
          continue;
        }

        if(!_waitForData) continue;

        if(oldgids.find(gid) != oldgids.end()) continue;

        const auto gid32 = static_cast<uint32_t>(gid);

        gids.push_back(gid32);

        // according to the model the collectionId is a number.
        const auto groupId = props["nodeCollectionId"].asUInt64();
        if (groupId > RANGE_LIMIT)
        {
          ++rangeErrors;
          continue;
        }
        const auto groupId32 = static_cast<uint32_t>(groupId);

        populationMap[std::to_string(groupId32)].push_back(gid32);

        const auto position = props["position"];
        if(props.isNull()) continue;

        positions.emplace_back(position[0].asFloat(),
                               position[1].asFloat(),
                               position[2].asFloat());
      }
    }
    catch (const std::exception &e)
    {
      auto message = std::string(e.what());
      std::replace(message.begin(), message.end(), '\n', ' ');
      std::cerr << "REST - NODES ERROR: callbackNodes Exception JSON PARSER (" << message << ")" << std::endl;
      std::cerr << "JSON ROOT is: " << root << std::endl;
      return RESTResult::EXCEPTION;
    }

    if(!_waitForData) return RESTResult::NODATA;

    if ( !gids.empty() )
    {
      _network->setNeurons( gids, positions );
    }

    if(gids.size() != positions.size())
    {
      std::cerr << "REST - NODES ERROR: callbackNodeProperties gids and positions mismatch (" << gids.size() << " != " << positions.size() << ")" << std::endl;
    }

    if ( !populationMap.empty() )
    {
      auto addSubset = [this](const std::pair<std::string, GIDVec> &item)
      {
        _network->subsetsEvents( )->addSubset( item.first, item.second );
      };
      std::for_each(populationMap.cbegin(), populationMap.cend(), addSubset);
    }

    if(rangeErrors > 0)
    {
      std::cerr << "REST - NODES ERROR: Node or group ids outside of uin32_t range (" << rangeErrors << ")" << std::endl;
    }

    return ( !gids.empty() || !populationMap.empty() ) ? RESTResult::NEWDATA : RESTResult::NODATA;
  }

  void LoaderRestData::loopSpikes( const std::string &url, const std::string &prefix, const unsigned int port )
  {
    while ( _waitForData )
    {
      const auto result = getSpikes( url, prefix, port );

      switch ( result )
      {
        case RESTResult::NOTCONNECTED:
          std::this_thread::sleep_for( std::chrono::milliseconds( m_config.failTime ) );
          break;
        case RESTResult::EXCEPTION:
          std::this_thread::sleep_for( std::chrono::milliseconds( m_config.failTime ) );
          break;
        case RESTResult::NODATA:
          std::this_thread::sleep_for( std::chrono::milliseconds( m_config.waitTime ) );
          break;
        case RESTResult::NEWDATA:
          std::this_thread::sleep_for( std::chrono::milliseconds( m_config.waitTime ) );
          break;
      }
    }
  }

  void LoaderRestData::loopNetwork( const std::string &url, const std::string &prefix, const unsigned int port )
  {
    // NOTE: aborts after getting node positions, as NEST doesn't add new nodes later.
    while ( _waitForData && _network->gidsSize() < 2)
    {
      const auto result = getNodeProperties( url, prefix, port );

      switch ( result )
      {
        case RESTResult::NOTCONNECTED:
          std::this_thread::sleep_for( std::chrono::milliseconds( m_config.failTime ) );
          break;
        case RESTResult::EXCEPTION:
          std::this_thread::sleep_for( std::chrono::milliseconds( m_config.failTime ) );
          break;
        case RESTResult::NODATA:
          std::this_thread::sleep_for( std::chrono::milliseconds( m_config.waitTime ) );
          break;
        case RESTResult::NEWDATA:
          std::this_thread::sleep_for( std::chrono::milliseconds( m_config.waitTime ) );
          break;
      }
    }
  }

  LoaderRestData::RESTResult LoaderRestData::getNodeProperties( const std::string &url, const std::string &prefix, const unsigned int port )
  {
    HTTPSyncClient client;

    client.set_host( url );
    client.set_uri( prefix + "/nodes" );
    client.set_port( port );
    const auto answer = client.execute();

    if ( answer == boost::system::errc::success ) // Success
    {
      return callbackNodeProperties( client.get_response( ) );
    }

    std::cerr << "REST - NODES ERROR: " << client.get_status_message() << std::endl;

    return RESTResult::NOTCONNECTED;
  }

  LoaderRestData::RESTResult LoaderRestData::getSpikes( const std::string &url, const std::string &prefix, const unsigned int port )
  {
    HTTPSyncClient client;

    std::string uri( prefix + "/spikes?" );
    if( _spikesRead > 0 )
    {
      uri.append("skip=" );
      uri.append( std::to_string( _spikesRead ) );
      uri.append("&");
    }
    uri.append( "top=" );
    uri.append( std::to_string( m_config.spikesSize ) );

    client.set_host( url );
    client.set_uri( uri );
    client.set_port( port );

    const auto answer = client.execute();
    if ( answer == boost::system::errc::success )
    {
      const auto result = callbackSpikes( client.get_response( ) );
      if ( result == RESTResult::NEWDATA )
      {
        const auto spikes = dynamic_cast< SpikeData* >( _simulationdata );
        if(spikes) _spikesRead = spikes->spikes().size();
      }

      return result;
    }

    std::cerr << "REST - SPIKES ERROR: " << client.get_status_message() << std::endl;

    return RESTResult::NOTCONNECTED;
  }

  std::string LoaderRestData::restAPIPrefix() const
  {
    return m_config.api == Rest_API::NEST ? NEST_PREFIX : ARBOR_PREFIX;
  }

  void LoaderRestData::setConfiguration(const Configuration &config)
  {
    const bool differentURL = (m_config.url != config.url);
    m_config = config;

    // NOTE: if running maybe the loop thread is stopped, so restart it.
    if(_waitForData && differentURL)
    {
      _spikesRead = 0;
      loadNetwork(m_config.url, std::to_string(m_config.port));
    }
  }

  LoaderRestData::Configuration LoaderRestData::getConfiguration() const
  {
    return m_config;
  }

} // namespace simil
