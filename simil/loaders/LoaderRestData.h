/*
 * @file  LoadInsituData.h
 * @brief
 * @author Aaron Sújar <aaron.sujar@urjc.es>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 *          Do not distribute without further notice.
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

  protected:
    enum GETRequest
    {
      TimeInfo = 0,
      Gids,
      NeuronPro,
      Populations,
      Spikes
    };

    void callbackSpikes( std::istream& contentdata );
    void callbackGids( std::istream& contentdata );
    void callbackPopulations( std::istream& contentdata );
    void callbackTime( std::istream& contentdata );
    void callbackNProperties( std::istream& contentdata );

    void loopSpikes( );
    void loopNetwork( );

    int GETTimeInfo( );
    int GETGids( );
    int GETNeuronProperties( );
    int GETPopulations( );
    int GETSpikes( );

    std::unique_ptr< LoaderRestData > _instance;
    std::thread _looperSpikes;
    std::thread _looperNetwork;
    SimulationData* _simulationdata;
    Network* _network;
    bool _waitForData;
    std::string _host;
    unsigned int _port;
    float _deltaTime;
  };

} // namespace simil

#endif /* __SIMIL__LOADRESTAPIDATA_H__ */
