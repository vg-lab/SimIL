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

#ifndef __SIMIL__LOADRESTAPIDATA_H__
#define __SIMIL__LOADRESTAPIDATA_H__

// Project
#include "LoaderSimData.h"
#include "simil/Network.h"
#include "simil/SpikeData.h"
#include <memory>
#include <simil/api.h>

// C++
#include <thread>
#include <atomic>

/** NOTES: updated to REST API 1.0 from https://github.com/VRGroupRWTH/insite/tree/develop/docs/api
 * NEST API only for now, pending ARBOR API and testing.
 *
 */
namespace simil
{
  class SIMIL_API LoaderRestData : public LoaderSimData
  {
  public:
    LoaderRestData( );

    virtual ~LoaderRestData( );

    virtual std::unique_ptr< SimulationData >
    loadSimulationData( const std::string& url ,
                        const std::string& port = "" ) override;

    virtual std::unique_ptr< Network >
    loadNetwork( const std::string& url ,
                 const std::string& port = "" ) override;

    /** \brief Implemented rest APIs.
     *
     */
    enum class Rest_API
    {
      NEST = 0 , ARBOR
    };

    /** \struct Configuration
     * \brief Implements the REST API configuration.
     *
     */
    struct Configuration
    {
      Rest_API api;        /** REST API, NEST or ARBOR.           */
      std::string url;        /** server url.                        */
      unsigned int port;       /** server port.                       */
      unsigned int waitTime;   /** wait time after a successful call. */
      unsigned int failTime;   /** wait time after a failed call.     */
      unsigned int spikesSize; /** amount of spikes to ask in a call. */
      std::weak_ptr<Network> network; /** If not null, the loader won't load a new network and will use this network instead */

      Configuration( )
        : api( Rest_API::NEST )
        , url( "localhost" )
        , port( 52056 )
        , waitTime( 5000 )
        , failTime( 1000 )
        , spikesSize( 1000 )
        , network()
      { };
    };

    /** \brief Sets the REST connection options.
     * \param[in] o Options struct reference.
     *
     */
    void setConfiguration( const Configuration& o );

    /** \brief Returns the REST connection options.
     *
     */
    Configuration getConfiguration( ) const;

    struct Version
    {
      std::string api;    /** the available endpoint versions for the REST API. */
      std::string insite; /** specifies the version of the insite pipeline. */
    };

    /** \brief Returns the api version of the connection.
     *
     */
    struct Version
    getVersion( const std::string url , const unsigned int port );

  protected:
    static const std::string ARBOR_PREFIX;   /** uri prefix to get arbor data from server. */
    static const std::string NEST_PREFIX;    /** uri prefix to get nest data from server.  */

    enum class RESTResultType
    {
      NOTCONNECTED = 0 ,
      EXCEPTION ,
      NODATA ,
      NEWDATA
    };

    struct RESTResult
    {
      RESTResultType type;
      bool stopThread;

      RESTResult( RESTResultType type_ , bool stopThread_ )
        : type( type_ )
        , stopThread( stopThread_ )
      {
      }
    };

    /** Callback methods for processing JSON contents.
     *
     */
    RESTResult callbackSpikes( SpikeData* spikes , std::istream& data ) const;

    RESTResult callbackNodeProperties( Network* network ,
                                       std::istream& contentdata );

    /** Calling methods to request data from server.
     *
     */
    void loopSpikes( SpikeData* data ,
                     const std::string& url , const std::string& prefix ,
                     const unsigned int port );

    void loopNetwork( Network* network ,
                      const std::string& url , const std::string& prefix ,
                      const unsigned int port );

    /** \brief Get the properties information from the server.
     * \param[in] url server address
     * \param[in] prefix server uri prefix.
     * \param[in] port server address port.
     */
    RESTResult
    getNodeProperties( Network* network ,
                       const std::string& url , const std::string& prefix ,
                       const unsigned int port );

    /** \brief Get spikes information from then server.
     * \param[in] url server address
     * \param[in] prefix server uri prefix.
     * \param[in] port server address port.
     *
     */
    RESTResult getSpikes( SpikeData* spikes ,
                          const std::string& url , const std::string& prefix ,
                          const unsigned int port );

    /** \brief Helper method to get the uri prefix depending on the rest API used.
     *
     */
    std::string restAPIPrefix( ) const;

    std::atomic< bool > _forceStop;
    std::atomic< unsigned int > _spikesRead;
    Configuration m_config;
  };

} // namespace simil

#endif /* __SIMIL__LOADRESTAPIDATA_H__ */
