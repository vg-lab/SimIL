/*
 * @file  LoadInsituData.h
 * @brief
 * @author Aaron Sújar <aaron.sujar@urjc.es>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 *          Do not distribute without further notice.
 */

#include "LoadRestApiData.h"
#include "../SimulationData.h"

#include <iostream>
#include <exception>
#include <curl/curl.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/range/combine.hpp>

namespace simil
{
  // Cone _cone;
  LoadRestApiData::LoadRestApiData( )
    : LoadSimData( )
    //, _dataset( nullptr )
    , _instance( nullptr )
    , _simulationdata( nullptr )
    , _waitForData( false )
    , _host( "localhost" )
    , _port( 8000 )
  {
  }

  LoadRestApiData::~LoadRestApiData( )
  {
    _waitForData = false;
    _spikeslooper.join( );
    _networklooper.join( );
  }

  SimulationData* LoadRestApiData::LoadSimulationData( const std::string&,
                                                       const std::string& )
  {
    if ( _simulationdata == nullptr )
      _simulationdata = new SpikeData( );

    _waitForData = true;
    _networklooper = std::thread( &LoadRestApiData::Networkloop, this );
    //_spikeslooper = std::thread( &LoadRestApiData::Spikeloop, this );

    return _simulationdata;
  } // namespace simil

  void LoadRestApiData::SpikeCB( std::istream& contentdata )
  {
    boost::property_tree::ptree propertytree;
    boost::property_tree::read_json( contentdata, propertytree );
    // neuron_ids
    // simulation_steps/

    SpikeData* _spikes = dynamic_cast< SpikeData* >( _simulationdata );

    float startTime = _spikes->startTime( );
    float endTime = _spikes->endTime( );

    auto it_gids = propertytree.get_child( "neuron_ids" ).begin( );
    auto it_times = propertytree.get_child( "simulation_steps" ).begin( );
    auto gids_end = propertytree.get_child( "neuron_ids" ).end( );

    for ( ; it_gids != gids_end; ++it_gids, ++it_times )
    {
      float timestamp = it_times->second.get_value< unsigned int >( ) * 0.1f;
      if ( timestamp < startTime )
        _spikes->setStartTime( timestamp );
      if ( timestamp > endTime )
      {
        _spikes->setEndTime( timestamp );
        _spikes->addSpike( timestamp,
                           it_gids->second.get_value< unsigned int >( ) );
      }
    }
  }

  void LoadRestApiData::handlerStatic( const HTTPRequest& request,
                                       HTTPResponse& response,
                                       system::error_code error_code )
  {
    if ( error_code == boost::system::errc::success )
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
          default:
            break;
        }
      }
      else
         std::cerr << "Status code:" <<response.get_status_message()
                   << ":" <<response.get_status_code( ) <<"\n";
    }
    else if ( error_code == asio::error::operation_aborted )
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
  }

  void LoadRestApiData::Spikeloop( )
  {
    while ( _waitForData )
    {
      GETSpikes( );
      std::this_thread::sleep_for( std::chrono::milliseconds( 300 ) );
    }
  }
  void LoadRestApiData::Networkloop( )
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
      GETGids( );
      GETPopulations( );

      std::this_thread::sleep_for( std::chrono::seconds( 3 ) );
      std::cout << "Intentamos otra vez\n";
    }
  }

  void LoadRestApiData::GETTimeInfo( )
  {
    try
    {
      std::cout << "Llamada Time\n";
      HTTPClient client;

      std::shared_ptr< HTTPRequest > request = client.create_request( 1 );

      request->set_host( _host );
      request->set_uri( "/simulation_time_info" );
      request->set_port( _port );
      request->set_callback( handlerStatic );
      request->set_handlerId( GETRequest::TimeInfo );
      request->set_userpointer( this );
      request->execute( );

      std::this_thread::sleep_for( std::chrono::milliseconds( 15 ) );

      client.close( );
    }
    catch ( system::system_error& e )
    {
      std::cout << "Error occured! Error code = " << e.code( )
                << ". Message: " << e.what( );
    }
  }
  void LoadRestApiData::GETGids( )
  {
    try
    {
      std::cout << "Llamada Gid\n";
      HTTPClient client;

      std::shared_ptr< HTTPRequest > request = client.create_request( 1 );

      request->set_host( _host );
      request->set_uri( "/gids" );
      request->set_port( _port );
      request->set_callback( handlerStatic );
      request->set_handlerId( GETRequest::Gids );
      request->set_userpointer( this );
      request->execute( );

      std::this_thread::sleep_for( std::chrono::milliseconds( 15 ) );

      client.close( );
    }
    catch ( system::system_error& e )
    {
      std::cout << "Error occured! Error code = " << e.code( )
                << ". Message: " << e.what( );
    }
  }
  void LoadRestApiData::GETNeuronProperties( )
  {
    try
    {
      std::cout << "Llamada Neuro\n";
      HTTPClient client;

      std::shared_ptr< HTTPRequest > request = client.create_request( 1 );

      request->set_host( _host );
      request->set_uri( "/neuron_properties" );
      request->set_port( _port );
      request->set_callback( handlerStatic );
      request->set_handlerId( GETRequest::NeuronPro );
      request->set_userpointer( this );
      request->execute( );

      std::this_thread::sleep_for( std::chrono::milliseconds( 15 ) );

      client.close( );
    }
    catch ( system::system_error& e )
    {
      std::cout << "Error occured! Error code = " << e.code( )
                << ". Message: " << e.what( );
    }
  }
  void LoadRestApiData::GETPopulations( )
  {
    try
    {
      std::cout << "Llamada Population\n";
      HTTPClient client;

      std::shared_ptr< HTTPRequest > request = client.create_request( 1 );

      request->set_host( _host );
      request->set_uri( "/populations" );
      request->set_port( _port );
      request->set_callback( handlerStatic );
      request->set_handlerId( GETRequest::Populations );
      request->set_userpointer( this );
      request->execute( );

      std::this_thread::sleep_for( std::chrono::milliseconds( 15 ) );

      client.close( );
    }
    catch ( system::system_error& e )
    {
      std::cout << "Error occured! Error code = " << e.code( )
                << ". Message: " << e.what( );
    }
  }
  void LoadRestApiData::GETSpikes( )
  {
    try
    {
      std::cout << "Llamada Spike\n";
      HTTPClient client;

      std::shared_ptr< HTTPRequest > request = client.create_request( 1 );

      request->set_host( _host );
      request->set_uri( "/spikes" );
      request->set_port( _port );
      request->set_callback( handlerStatic );
      request->set_handlerId( GETRequest::Spikes );
      request->set_userpointer( this );
      request->execute( );

      std::this_thread::sleep_for( std::chrono::milliseconds( 15 ) );

      client.close( );
    }
    catch ( system::system_error& e )
    {
      std::cout << "Error occured! Error code = " << e.code( )
                << ". Message: " << e.what( );
    }
  }

} // namespace simil
