/*
 * Copyright (c) 2015-2020 VG-Lab/URJC.
 *
 * Authors: Sergio E. Galindo <sergio.galindo@urjc.es>
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

#include "SubsetEventManager.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/lexical_cast.hpp>

#include <queue>
#include <set>

#include "loaders/auxiliar/H5SubsetEvents.h"

namespace simil
{
  void getLeafNodesPath( const boost::property_tree::ptree& tree,
                         std::string currentPath,
                         std::vector< std::string >& leafNodes )
  {
    if( tree.empty( ))
      leafNodes.push_back( currentPath );
    else
    {
      for( auto& child : tree )
        getLeafNodesPath( child.second,
                          currentPath + ( !currentPath.empty( ) ? "." : "") + child.first,
                          leafNodes );
    }
  }

  void split( const std::string& s,
              char delim,
              std::vector< std::string >& elems,
              bool skipEmpty )
  {
      std::stringstream ss;
      ss.str( s );
      std::string item;
      while( std::getline( ss, item, delim ))
        if( !skipEmpty || ( skipEmpty && !item.empty( )))
          elems.push_back( item );
  }

  std::vector< std::string > split( const std::string &s,
                                    char delim,
                                    bool skipEmpty = true )
  {
    std::vector< std::string > elems;
    split( s, delim, elems, skipEmpty );
    return elems;
  }

  std::set< uint32_t > parseGIDsJSON( const std::string& stringGIDs )
  {
    std::set< uint32_t > result;

    std::vector< std::string > tokens = split( stringGIDs, ',', true );

    std::vector< std::string > range;
    for( auto& token : tokens )
    {
      if( token.find( ":" ) != std::string::npos )
      {
        range = split( token, ':', false );

        uint32_t lowerLimit = 0;
        uint32_t upperLimit = 0;

        try
        {
          if( !range[ 0 ].empty( ))
            lowerLimit = boost::lexical_cast< uint32_t >( range[ 0 ]);

          upperLimit = boost::lexical_cast< uint32_t >( range[ 1 ]);

          for( unsigned int i = lowerLimit; i < upperLimit; i++ )
            result.insert( i );
        }
        catch(const std::exception& e)
        {
          const std::string errorText = std::string(e.what()) + "\nCouldn't parse: " +
                                        range[0] + " or " + range[1];
          std::cerr << "EXCEPTION: " << errorText << " -> " << __FILE__ << ":" << __LINE__ << std::endl;

          throw std::runtime_error(errorText.c_str());
        }
      }
      else
      {
        try
        {
          unsigned int value = boost::lexical_cast< uint32_t >( token );
          result.insert( value );
        }
        catch(const std::exception& e)
        {
          const std::string errorText = std::string(e.what()) + "\nCouldn't parse: " + token;
          std::cerr << "EXCEPTION: " << errorText << " -> " << __FILE__ << ":" << __LINE__ << std::endl;

          throw std::runtime_error(errorText.c_str());
        }
      }
    }

    return result;
  }

  std::vector< Event > parseTimeFrameJSON( const std::string& stringTimeFrames )
  {
    std::vector< Event >result;

    std::vector< std::string > timeFrames =
      split( stringTimeFrames, ';', false );

    for( auto timeFrameString : timeFrames )
    {
      Event timeFrame;

      std::vector< std::string > range =
        split( timeFrameString, ':', false );

      try
      {
        if( !range[ 0 ].empty( ))
          timeFrame.first = boost::lexical_cast< float >( range[ 0 ]);

        timeFrame.second = boost::lexical_cast< float >( range[ 1 ]);
      }
      catch(const std::exception &e)
      {
        const std::string errorText = std::string(e.what()) + "\nCouldn't parse: " +
                                      range[0] + " or " + range[1];
        std::cerr << "EXCEPTION: " << errorText << " -> " << __FILE__ << ":" << __LINE__ << std::endl;

        throw std::runtime_error(errorText.c_str());
      }

      result.push_back( timeFrame );
    }

    return result;
  }

  SubsetEventManager::SubsetEventManager( )
  : _totalTime( 0.0f )
  { }

  void SubsetEventManager::loadJSON( const std::string& filePath )
  {
    try
    {
      using namespace boost::property_tree;

      ptree pt;
      read_json( filePath, pt );

      for( auto& subset : pt.get_child( "subsets" ))
      {
        for( auto& child : subset.second )
        {
          std::set< uint32_t > gidSet =
            parseGIDsJSON( child.second.get_value< std::string >( ));

          GIDVec gids( gidSet.begin( ), gidSet.end( ));

          _subsets.insert( std::make_pair( child.first, gids ));
        }
      }

      for( auto& timeframe : pt.get_child( "timeframes" ))
      {
        for( auto& child : timeframe.second )
        {
          std::vector< Event > timeFrame =
            parseTimeFrameJSON( child.second.get_value< std::string >( ));

          _events.insert( std::make_pair( child.first, timeFrame ));
        }
      }
    }
    catch( std::exception& e )
    {
      const std::string errorText = std::string(e.what()) + "\nCouldn't load JSON: " + filePath;
      std::cerr << "EXCEPTION: " << errorText << " -> " << __FILE__ << ":" << __LINE__ << std::endl;

      throw std::runtime_error(errorText.c_str());
    }
  }

  void SubsetEventManager::loadH5( const std::string& filePath)
  {
    H5SubsetEvents reader;

    reader.Load(filePath, "length", "pattern_activation");

    for (auto &subset : reader.subsets())
      addSubset(subset.name, subset.gids);

    for (auto &tf : reader.timeFrames())
      _events.insert(std::make_pair(tf.name, tf.timeFrames));

    _totalTime = reader.totalTime();
  }

  void SubsetEventManager::clear( void )
  {
    _subsets.clear( );
    _events.clear( );
    _colors.clear();
  }

  std::vector< uint32_t >
  SubsetEventManager::getSubset( const std::string& name ) const
  {
    std::vector<uint32_t> result;

    auto it = _subsets.find(name);
    if (it != _subsets.end())
      return it->second;

    return result;
  }

  vmml::Vector3f SubsetEventManager::getSubsetColor(const std::string &name) const
  {
    auto it = _colors.find(name);
    if(it != _colors.end())
      return it->second;

    return vmml::Vector3f{0,0,0};
  }

  void SubsetEventManager::addSubset( const std::string& name,
                                      const GIDVec& subset,
                                      const vmml::Vector3f& color)
  {
    if (_subsets.count(name) > 0)
    {
      GIDVec &gidVecs = _subsets[name];
      gidVecs.insert(gidVecs.end(), subset.begin(), subset.end());

      std::vector<uint32_t>::iterator ip;
      ip = std::unique(gidVecs.begin(), gidVecs.end());
      gidVecs.resize(std::distance(gidVecs.begin(), ip));
    }
    else
    {
      _subsets.insert( std::make_pair( name, subset ));
      _colors.insert(std::make_pair(name, color));
    }
  }

  void SubsetEventManager::removeSubset( const std::string& name )
  {
    _subsets.erase( name );
    _colors.erase(name);
  }

  std::vector< Event >
  SubsetEventManager::getEvent( const std::string& name ) const
  {
    std::vector< Event > result;

    auto it = _events.find( name );

    if( it != _events.end( ))
      result = it->second;

    return result;
  }

  SubsetMapRange SubsetEventManager::subsets( void ) const
  {
    return std::make_pair( _subsets.begin( ), _subsets.end( ));
  }

  EventRange SubsetEventManager::events( void ) const
  {
    return std::make_pair( _events.begin( ), _events.end( ));
  }

  unsigned int SubsetEventManager::numSubsets( void ) const
  {
    return static_cast<unsigned int>(_subsets.size());
  }

  unsigned int SubsetEventManager::numEvents( void ) const
  {
    return static_cast<unsigned int>(_events.size());
  }

  float SubsetEventManager::totalTime( void ) const
  {
    return _totalTime;
  }

  std::vector< std::string > SubsetEventManager::subsetNames( void ) const
  {
    std::vector<std::string> result;

    for(auto subset: _subsets)
      result.push_back(subset.first);

    return result;
  }

  std::vector< std::string > SubsetEventManager::eventNames( void ) const
  {
    std::vector<std::string> result;

    for(auto event: _events)
      result.push_back(event.first);

    return result;
  }

  std::vector< bool > SubsetEventManager::eventActivity( const std::string& name,
                                                         float deltaTime,
                                                         float totalTime ) const
  {
    std::vector< bool > result;

    EventVec eventVec = getEvent( name );

    if(eventVec.empty())
    {
      std::cout << "Warning: event " << name << " NOT found." << std::endl;
      return result;
    }

    // Calculate delta time inverse to avoid further division operations.
    const double invDeltaTime = 1.0 / deltaTime;

    // Threshold for considering an event active during bin time.
    const float threshold = deltaTime * 0.5f;

    // Calculate bins number.
    const unsigned int binsNumber = std::ceil( totalTime * invDeltaTime );

    result.resize( binsNumber, false );

    std::vector< float > eventTime( binsNumber, 0.0f );

    for( const auto& event : eventVec )
    {
      float acc = 0.0f;
      unsigned int counter = 0;

      unsigned int binStart = std::floor( event.first * invDeltaTime );
      unsigned int binEnd = std::ceil( event.second * invDeltaTime );

      if( binEnd > binsNumber )
        binEnd = binsNumber;

      if( binStart > binEnd )
        continue;

      acc = binStart * deltaTime;

      for( auto binIt = eventTime.begin( ) + binStart;
           binIt != eventTime.begin( ) + binEnd; ++binIt )
      {

        const auto lowerBound = (std::max)( acc, event.first );
        const auto upperBound = (std::min)( acc + deltaTime, event.second );

        *binIt += ( upperBound - lowerBound );

        acc += deltaTime;
        counter++;
      }

      auto bin = result.begin( );
      for( auto time : eventTime )
      {
        if( time >= threshold )
          *bin = true;

        ++bin;
      }
    }

    return result;
  }
}

