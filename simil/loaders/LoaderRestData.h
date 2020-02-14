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

#ifndef __SIMIL__LOADRESTAPIDATA_H__
#define __SIMIL__LOADRESTAPIDATA_H__

#include "LoaderSimData.h"

#include <mutex>
#include <thread>

namespace simil
{
  class LoaderRestData : public LoaderSimData
  {
  public:
    LoaderRestData( );
    ~LoaderRestData( );
    virtual SimulationData*
      loadSimulationData( const std::string& hostURL,
                          const std::string& port ="" ) override;

    virtual Network* loadNetwork( const std::string& hostURL,
                                  const std::string& port ="" ) override;

    void network( Network* network );

    void deltaTime( float deltaTime);
    float deltaTime();

    void dataOffset( unsigned int offset);
    unsigned int dataOffeset();

  protected:
    enum GETRequest
    {
      TimeInfo = 0,
      Gids,
      NeuronPro,
      Populations,
      Spikes
    };

    enum RESTERROR
    {
      NOTCONNECT = -2,
      EXCEPTION = -1,
      NODATA = 0,
      NEWDATA = 1
    };

    int callbackSpikes( std::istream& contentdata );
    int callbackGids( std::istream& contentdata );
    int callbackPopulations( std::istream& contentdata );
    int callbackTime( std::istream& contentdata );
    int callbackNProperties( std::istream& contentdata );

    void loopSpikes( );
    void loopNetwork( );

    int GETTimeInfo( );
    int GETGids( );
    int GETNeuronProperties(  );
    int GETPopulations(  );
    int GETSpikes();

    std::unique_ptr< LoaderRestData > _instance;
    std::thread _looperSpikes;
    std::thread _looperNetwork;
    SimulationData* _simulationdata;
    Network* _network;
    bool _waitForData;
    std::string _host;
    unsigned int _port;
    float _deltaTime;
    unsigned int _dataOffset;

    unsigned int _spikesRead;


  };

} // namespace simil

#endif /* __SIMIL__LOADRESTAPIDATA_H__ */
