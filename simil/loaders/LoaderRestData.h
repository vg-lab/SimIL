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

#include "LoaderSimData.h"

#include <mutex>
#include <thread>
#include <simil/api.h>

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
    ~LoaderRestData( );
    virtual SimulationData*
      loadSimulationData( const std::string& url,
                          const std::string& port = "" ) override;

    virtual Network* loadNetwork( const std::string& url,
                                  const std::string& port ="" ) override;

    void network( Network* network );

    void deltaTime( float deltaTime);
    float deltaTime();

    void dataOffset( unsigned int offset);
    unsigned int dataOffeset();

    /** \brief Implemented rest APIs.
     *
     */
    enum class Rest_API { NEST = 0, ARBOR };

    /** \brief Sets the rest API to use for data retrieval from server.
     *
     */
    inline void setRestAPI( const Rest_API value )
    { _api = value; }

  protected:
    static const std::string ARBOR_PREFIX;   /** uri prefix to get arbor data from server. */
    static const std::string NEST_PREFIX;    /** uri prefix to get nest data from server.  */

    enum class RESTResult
    {
      NOTCONNECT = 0,
      EXCEPTION,
      NODATA,
      NEWDATA
    };

    /** Callback methods for processing JSON contents.
     *
     */
    RESTResult callbackSpikes( std::istream& contentdata );
    RESTResult callbackNodeProperties( std::istream& contentdata );

    /** Calling methods to request data from server.
     *
     */
    void loopSpikes( const std::string &url, const std::string &prefix, const unsigned int port );
    void loopNetwork( const std::string &url, const std::string &prefix, const unsigned int port );

    /** \brief Get the properties information from the server.
     * \param[in] url server address
     * \param[in] prefix server uri prefix.
     * \param[in] port server address port.
     */
    RESTResult GETNodeProperties( const std::string& url, const std::string &prefix, const unsigned int port );

    /** \brief Get spikes information from then server.
     * \param[in] url server address
     * \param[in] prefix server uri prefix.
     * \param[in] port server address port.
     *
     */
    RESTResult GETSpikes( const std::string& url, const std::string &prefix, const unsigned int port );

    /** \brief Helper method to get the uri prefix depending on the rest API used.
     *
     */
    std::string restAPIPrefix() const;

    std::unique_ptr< LoaderRestData > _instance;
    std::thread _looperSpikes;
    std::thread _looperNetwork;
    SimulationData* _simulationdata;
    Network* _network;
    bool _waitForData;
    float _deltaTime;
    unsigned int _dataOffset;
    unsigned int _spikesRead;
    Rest_API _api;
  };

} // namespace simil

#endif /* __SIMIL__LOADRESTAPIDATA_H__ */
