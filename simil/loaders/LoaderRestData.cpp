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

#include "LoaderRestData.h"
#include "../SimulationData.h"

#include "HTTP/SyncClient.h"

#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

constexpr uint64_t RANGE_LIMIT = std::numeric_limits<uint32_t>::max();

namespace simil
{
  const std::string LoaderRestData::ARBOR_PREFIX   = "/arbor";
  const std::string LoaderRestData::NEST_PREFIX    = "/nest";

  const std::string DEFAULT_URL = "localhost";
  constexpr unsigned int DEFAULT_PORT = 8080;

  LoaderRestData::LoaderRestData( )
  : LoaderSimData( )
  , _instance{ nullptr }
  , _simulationdata{ nullptr }
  , _network{ nullptr }
  , _waitForData{ false }
  , _deltaTime{ 0.1f }
  , _dataOffset{ 1000 }
  , _spikesRead{ 0 }
  , _api{ Rest_API::NEST }
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
    auto serverUrl  = DEFAULT_URL;
    unsigned int serverPort = DEFAULT_PORT;

    if ( !port.empty( ) )
      serverPort = std::stoi(port);

    if( !url.empty() )
      serverUrl = url;

    if (!_simulationdata)
      _simulationdata = new SpikeData( );

    _waitForData = true;
    _looperSpikes = std::thread( &LoaderRestData::loopSpikes, this,
                                 serverUrl, restAPIPrefix(), serverPort );

    return _simulationdata;
  }

  Network* LoaderRestData::loadNetwork( const std::string& url,
                                        const std::string& port )
  {
    auto serverUrl  = DEFAULT_URL;
    unsigned int serverPort = DEFAULT_PORT;

    if ( !port.empty( ) )
      serverPort = std::stoi(port);

    if( !url.empty() )
      serverUrl = url;

    if (!_network)
      _network = new Network( );

    _waitForData = true;
    _looperNetwork = std::thread( &LoaderRestData::loopNetwork, this,
                                  serverUrl, restAPIPrefix(), serverPort );

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

  LoaderRestData::RESTResult LoaderRestData::callbackSpikes( std::istream& contentdata )
  {
    if(contentdata.eof() || contentdata.fail() || !_waitForData) return RESTResult::NODATA;

    SpikeData* _spikes = dynamic_cast< SpikeData* >( _simulationdata );
    TSpikes vecSpikes;
    float startTime, endTime;
    unsigned long rangeErrors = 0;

    boost::property_tree::basic_ptree<std::string, std::string> propertytree;
    try
    {
      boost::property_tree::read_json(contentdata, propertytree);

      if(!_waitForData) return RESTResult::NODATA;

      startTime = _spikes->startTime();
      endTime = _spikes->endTime();

      const auto ids = propertytree.get_child("nodeIds");
      const auto times = propertytree.get_child("simulationTimes");

      if(!ids.empty() && !times.empty())
      {
        auto timesIt = times.begin();
        vecSpikes.reserve(_dataOffset);

        for (auto gIt = ids.begin(); gIt != ids.end(); ++gIt, ++timesIt)
        {
          const auto nodeId = (*gIt).second.get_value<uint64_t>();

          if (nodeId > RANGE_LIMIT)
          {
            ++rangeErrors;
            continue;
          }

          const auto timestamp = static_cast<float>((*timesIt).second.get_value<double>());

          startTime = std::min(timestamp, startTime);
          endTime = std::max(timestamp, endTime);

          if (timestamp == endTime)
          {
            vecSpikes.push_back(
                std::make_pair(timestamp, static_cast<uint32_t>(nodeId)));
          }
        }
      }
    }
    catch (const std::exception &e)
    {
      // @felix boost parser is known to be unreliable, upgrade if possible
      std::cerr << "callbackSpikes Exception JSON PARSER:  " << e.what() << std::endl;
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
      std::cerr << "Node ids outside of uin32_t range: " << rangeErrors << std::endl;
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

    boost::property_tree::basic_ptree<std::string, std::string> propertytree;
    try
    {
      boost::property_tree::read_json(contentdata, propertytree);

      if(!_waitForData) return RESTResult::NODATA;

      auto &oldgids = _network->gids();

      // each propertyItem is a pair of property name and value.
      using propertyItem = std::pair<std::string, boost::property_tree::ptree>;

      auto insertProperty = [&](const propertyItem &prop)
      {
        const auto gid = prop.second.get_child("nodeId").get_value<uint64_t>();
        if (gid > RANGE_LIMIT)
        {
          ++rangeErrors;
          return;
        }

        if(!_waitForData) return;

        if(oldgids.find(gid) != oldgids.end()) return;

        const auto gid32 = static_cast<uint32_t>(gid);

        gids.push_back(gid32);

        // according to the model the collectionId is a number.
        const auto groupId = prop.second.get_child("nodeCollectionId").get_value<
            uint64_t>();
        if (groupId > RANGE_LIMIT)
        {
          ++rangeErrors;
          return;
        }
        const auto groupId32 = static_cast<uint32_t>(groupId);

        populationMap[std::to_string(groupId32)].push_back(gid32);

        const auto position = prop.second.get_child("position");

        int i = 0;
        float pos[3] { 0, 0, 0 };
        auto addPosition = [&i, &pos](const propertyItem &posIt)
        {
          if (i < 3)
            pos[i++] = static_cast<float>(posIt.second.get_value<double>());
        };
        std::for_each(position.begin(), position.end(), addPosition);

        positions.emplace_back(pos[0], pos[1], pos[2]);
      };
      std::for_each(propertytree.begin(), propertytree.end(), insertProperty);
    }
    catch (const std::exception &e)
    {
      // @felix boost parser is known to be unreliable, upgrade if possible
      std::cerr << "callbackNodes Exception JSON PARSER:  " << e.what() << std::endl;
      return RESTResult::EXCEPTION;
    }

    if(!_waitForData) return RESTResult::NODATA;

    if ( !gids.empty() )
    {
      _network->setNeurons( gids, positions );
    }

    if(gids.size() != positions.size())
    {
      std::cerr << "callbackNodeProperties: gids and positions mismatch (" << gids.size() << " != " << positions.size() << ")" << std::endl;
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
      std::cerr << "Node or group ids outside of uin32_t range: " << rangeErrors << std::endl;
    }

    return ( !gids.empty() || !populationMap.empty() ) ? RESTResult::NEWDATA : RESTResult::NODATA;
  }

  void LoaderRestData::loopSpikes( const std::string &url, const std::string &prefix, const unsigned int port )
  {
    while ( _waitForData )
    {
      const auto result = GETSpikes( url, prefix, port );

      switch ( result )
      {
        case RESTResult::NOTCONNECT:
          std::this_thread::sleep_for( std::chrono::milliseconds( 5000 ) );
          break;
        case RESTResult::EXCEPTION:
          std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ) );
          break;
        case RESTResult::NODATA:
          std::this_thread::sleep_for( std::chrono::milliseconds( 5000 ) );
          break;
        case RESTResult::NEWDATA:
          std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ) );
          break;
      }
    }
  }

  void LoaderRestData::loopNetwork( const std::string &url, const std::string &prefix, const unsigned int port )
  {
    while ( _waitForData )
    {
      const auto result = GETNodeProperties( url, prefix, port );

      switch ( result )
      {
        case RESTResult::NOTCONNECT:
          std::this_thread::sleep_for( std::chrono::milliseconds( 5000 ) );
          break;
        case RESTResult::EXCEPTION:
          std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ) );
          break;
        case RESTResult::NODATA:
          std::this_thread::sleep_for( std::chrono::milliseconds( 5000 ) );
          break;
        case RESTResult::NEWDATA:
          std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ) );
          break;
      }
    }
  }

  LoaderRestData::RESTResult LoaderRestData::GETNodeProperties( const std::string &url, const std::string &prefix, const unsigned int port )
  {
    HTTPSyncClient client;

    client.set_host( url );
    client.set_uri( prefix + "/nodes" );
    client.set_port( port );

    if ( boost::system::errc::success == client.execute( ) ) // Success
    {
      return callbackNodeProperties( client.get_response( ) );
    }

    return RESTResult::NOTCONNECT;
  }

  LoaderRestData::RESTResult LoaderRestData::GETSpikes( const std::string &url, const std::string &prefix, const unsigned int port )
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
    uri.append( std::to_string( _dataOffset ) );

    client.set_host( url );
    client.set_uri( uri );
    client.set_port( port );

    RESTResult result = RESTResult::NOTCONNECT;

    if ( boost::system::errc::success == client.execute( ) ) // Success
    {
      result = callbackSpikes( client.get_response( ) );
      if ( result == RESTResult::NEWDATA )
      {
        SpikeData* _spikes = dynamic_cast< SpikeData* >( _simulationdata );
        _spikesRead = _spikes->spikes().size();
      }
    }

    return result;
  }

  std::string LoaderRestData::restAPIPrefix() const
  {
    return _api == Rest_API::NEST ? NEST_PREFIX : ARBOR_PREFIX;
  }

} // namespace simil
