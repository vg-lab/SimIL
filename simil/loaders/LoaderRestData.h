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
