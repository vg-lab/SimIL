/*
 * @file  LoadInsituData.h
 * @brief
 * @author Aaron Sújar <aaron.sujar@urjc.es>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 *          Do not distribute without further notice.
 */

#include "LoaderRestData.h"
#include "../SimulationData.h"

#include "HTTP/SyncClient.h"

#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace simil
{
  // Cone _cone;
  LoaderRestData::LoaderRestData( )
    : LoaderSimData( )
    //, _dataset( nullptr )
    , _instance( nullptr )
    , _simulationdata( nullptr )
    , _network ( nullptr )
    , _waitForData( false )
    , _host( "localhost" )
    , _port( 8080 )
    , _deltaTime(0.1f)
  {
  }

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
      if (port.empty())
          _port =8080;
      else
          _port= static_cast<unsigned int>(std::stoi( port ));

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
    if (port.empty())
        _port =8080;
    else
        _port= static_cast<unsigned int>(std::stoi( port ));

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

  void LoaderRestData::deltaTime( float deltaTime)
  {
      _deltaTime = deltaTime;
  }

  float LoaderRestData::deltaTime( )
  {
      return _deltaTime;
  }

  void LoaderRestData::callbackSpikes( std::istream& contentdata )
  {

    boost::property_tree::ptree propertytree;
    try
    {
      boost::property_tree::read_json( contentdata, propertytree );
    }
    catch ( std::exception& e )
    {
      std::cerr << "Spikes Exception JSON PARSER:  " << e.what( )  <<"\n";
      return;
    }

    SpikeData* _spikes = dynamic_cast< SpikeData* >( _simulationdata );

    float startTime = _spikes->startTime( );
    float endTime = _spikes->endTime( );

    auto it_gids = propertytree.get_child( "gids" ).begin( );
    auto it_times = propertytree.get_child( "simulation_times" ).begin( );
    auto gids_end = propertytree.get_child( "gids" ).end( );

    TSpikes vecSpikes;
    vecSpikes.reserve(100);//Probably we can change it for N in order to ask N in the http request

    for ( ; it_gids != gids_end; ++it_gids, ++it_times )
    {
      float timestamp = it_times->second.get_value< float >( )* _deltaTime;
      if ( timestamp < startTime )
        startTime = timestamp;
      if ( timestamp > endTime )
      {
        endTime = timestamp;
        vecSpikes.push_back(std::make_pair( timestamp,
                           it_gids->second.get_value< unsigned int >( ) ));
      }
    }

    if (vecSpikes.size()>0)
    {
        _spikes->addSpikes(vecSpikes);
        _spikes->setStartTime(startTime);
        _spikes->setEndTime(endTime);
    }

  }

  void LoaderRestData::callbackGids( std::istream& contentdata )
  {
    boost::property_tree::ptree propertytree;
    try
    {
      boost::property_tree::read_json( contentdata, propertytree );
    }
    catch ( std::exception& e )
    {
      std::cerr << "Gids Exception JSON PARSER:  " << e.what( ) << "\n";
      return;
    }

    auto it_gids = propertytree.begin( );
    auto gids_end = propertytree.end( );

    TGIDSet gidSet;

    for ( ; it_gids != gids_end; ++it_gids )
    {
      float number = it_gids->second.get_value< float >( );
      gidSet.insert( static_cast< unsigned int >( number ) );
    }
    if (gidSet.size() >0)
        _network->setGids( gidSet, true );
  }

  void LoaderRestData::callbackPopulations( std::istream& contentdata )
  {
    boost::property_tree::ptree propertytree;
    try
    {
      boost::property_tree::read_json( contentdata, propertytree );
    }
    catch ( std::exception& e )
    {
      std::cerr << "Exception JSON PARSER:  " << e.what( ) << "\n";
      std::cerr << contentdata.rdbuf();
      return;
    }
  }

  void LoaderRestData::callbackNProperties( std::istream& contentdata )
  {
    boost::property_tree::ptree propertytree;
    try
    {
      boost::property_tree::read_json( contentdata, propertytree );
    }
    catch ( std::exception& e )
    {
      std::cerr << "NProp Exception JSON PARSER:  " << e.what( ) << "\n";
      //std::cerr << contentdata.rdbuf();
      return;
    }

    auto it_neuron = propertytree.begin( );
    auto it_end = propertytree.end( );

    SubsetMap populationMap;
    TGIDVect gids;
    TPosVect positions;
    float pos[3]{ 0, 0, 0 };

    for ( ; it_neuron != it_end; ++it_neuron )
    {

       unsigned int gid = it_neuron->second.get_child("gid").get_value<unsigned int>();
       gids.push_back(gid);

       auto properties = it_neuron->second.get_child("properties");

       std::string group = properties.get_child("population").get_value<std::string>();
       populationMap[group].push_back(gid);

       auto position = properties.get_child("position");

       int i = 0;
       for ( auto it = position.begin();it!=position.end();++it)
       {
          pos[i] = it->second.get_value<float>();
          ++i;
       }
       vmml::Vector3f x(pos[0],pos[1],pos[2]);
       positions.push_back(x);
    }


    _network->setNeurons(gids,positions);
    for(auto it = populationMap.begin();it!=populationMap.end();++it)
    {
        _network->subsetsEvents()->addSubset(it->first,it->second);
    }



  }

  void LoaderRestData::callbackTime( std::istream& contentdata )
  {
    boost::property_tree::ptree propertytree;
    try
    {
      boost::property_tree::read_json( contentdata, propertytree );
    }
    catch ( std::exception& e )
    {
      std::cerr << "Exception JSON PARSER:  " << e.what( ) << "\n";
      std::cerr << contentdata.rdbuf();
      return;
    }
  }

  /*void LoadRestApiData::handlerStatic( const HTTPRequest& request,
                                       HTTPResponse& response,
                                       boost::system::error_code error_code )
  {
    if ( error_code == 0 )
    {
      if ( response.get_status_code( ) == 200 )
      {
        std::cout << "Request #" << request.get_id( )
                  << " has completed. Response: ";
        //<< response.get_response().rdbuf();
        LoadRestApiData* loader =
          static_cast< LoadRestApiData* >( request.get_userpointer( ) );

        switch ( request.get_handlerId( ) )
        {
          case GETRequest::Spikes:
            loader->SpikeCB( response.get_response( ) );
            break;
          case GETRequest::Gids:
            loader->GidsCB( response.get_response( ) );
            break;
          case GETRequest::NeuronPro:
            loader->NPropertiesCB( response.get_response( ) );
            break;
          case GETRequest::TimeInfo:
            loader->TimeCB( response.get_response( ) );
            break;
          case GETRequest::Populations:
            loader->PopulationsCB( response.get_response( ) );
            break;
          default:
            break;
        }
      }
      else
        std::cerr << "Status code:" << response.get_status_message( ) << ":"
                  << response.get_status_code( ) << "\n";
    }
    else if ( error_code == boost::asio::error::operation_aborted )
    {
      std::cerr << "Request #" << request.get_id( )
                << " has been cancelled by the user." << std::endl;
    }
    else
    {
      std::cerr << "Request #" << request.get_id( )
                << " failed! Error code = " << error_code.value( )
                << ". Error message = " << error_code.message( ) << std::endl;
    }
  }*/

  void LoaderRestData::loopSpikes( )
  {
    while ( _waitForData )
    {
      if ( GETSpikes( ) == 0 )
        std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ) );
      else
        std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ) );

      //SpikeData* _spikes = dynamic_cast< SpikeData* >( _simulationdata );
      //std::cout << "Loaded spikes: " << _spikes->spikes( ).size( ) << std::endl;
    }
  }
  void LoaderRestData::loopNetwork( )
  {


    while ( _waitForData )
    {
      /*auto gids = NetData.GetNeuronIds( );
      auto numGids = gids.number_of_elements( );
      for ( unsigned int i = 0; i < numGids; i++ )
      {
        _simulationdata->setGid( gids[ i ] );
      }

      auto pos = NetData.GetFloatingPointAttributeValues( "Positions" );
      auto numPos = pos.number_of_elements( );
      for ( unsigned int i = 0; i < numPos; i++ )
      {
        _simulationdata->setPosition(
          vmml::Vector3f( pos[ i ], pos[ i ], pos[ i ] ) );
      }*/

      GETTimeInfo( );
      //GETGids( );
      GETNeuronProperties();
      //GETPopulations( );

      //std::cout << "Loaded gids: " << _network->gids( ).size( ) << std::endl;


      std::this_thread::sleep_for( std::chrono::milliseconds( 30000 ) );
    }
  }

  int LoaderRestData::GETTimeInfo( )
  {
    HTTPSyncClient client;

    client.set_host( _host );
    client.set_uri( "/simulation_time_info" );
    client.set_port( _port );

    int result = client.execute( );

    if ( result == 0 ) // Success
    {
      callbackTime( client.get_response( ) );
    }

    return result;
  }
  int LoaderRestData::GETGids( )
  {
    HTTPSyncClient client;

    client.set_host( _host );
    client.set_uri( "/gids" );
    client.set_port( _port );

    int result = client.execute( );

    if ( result == 0 ) // Success
    {
      callbackGids( client.get_response( ) );
    }
    return result;
  }
  int LoaderRestData::GETNeuronProperties( )
  {
    HTTPSyncClient client;

    client.set_host( _host );
    client.set_uri( "/neuron_properties" );
    client.set_port( _port );

    int result = client.execute( );

    if ( result == 0 ) // Success
    {
      callbackNProperties( client.get_response( ) );
    }
    return result;
  }
  int LoaderRestData::GETPopulations( )
  {
    HTTPSyncClient client;

    client.set_host( _host );
    client.set_uri( "/populations" );
    client.set_port( _port );

    int result = client.execute( );

    if ( result == 0 ) // Success
    {
      callbackPopulations( client.get_response( ) );
    }

    return result;
  }
  int LoaderRestData::GETSpikes( )
  {
    HTTPSyncClient client;

    client.set_host( _host );
    client.set_uri( "/spikes" );
    client.set_port( _port );

    int result = client.execute( );

    if ( result == 0 ) // Success
    {
      callbackSpikes( client.get_response( ) );
    }

    return result;
  }

} // namespace simil
