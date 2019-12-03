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

#include "LoadSimData.h"
#include "HTTPClient.hpp"

#include <mutex>
#include <thread>
#include <mutex>

namespace simil
{
  class LoadRestApiData : public LoadSimData
  {
  public:
    LoadRestApiData( );
    ~LoadRestApiData( );
    virtual SimulationData*
      LoadSimulationData( const std::string& filePath_,
                          const std::string& target = "" ) override;
  protected:

    enum GETRequest{
        TimeInfo = 0,
        Gids,
        NeuronPro,
        Populations,
        Spikes
    };

    void SpikeCB(std::istream& contentdata);
    void GidsCB(std::istream& contentdata);
    void PopulationsCB(std::istream& contentdata);
    void TimeCB(std::istream& contentdata);
    void NPropertiesCB(std::istream& contentdata);


    void Spikeloop( );
    void Networkloop( );

    void GETTimeInfo();
    void GETGids();
    void GETNeuronProperties();
    void GETPopulations();
    void GETSpikes();

    static void handlerStatic(const HTTPRequest& request,
                                HTTPResponse& response,
                                system::error_code ec);


    std::unique_ptr<LoadRestApiData> _instance;
    std::thread _spikeslooper;
    std::thread _networklooper;
    SimulationData* _simulationdata;
    bool _waitForData;
    std::string _host;
    unsigned int _port;

  };

} // namespace simil

#endif /* __SIMIL__LOADRESTAPIDATA_H__ */
