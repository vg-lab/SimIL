/*
 * @file  LoadInsituData.h
 * @brief
 * @author Aaron Sújar <aaron.sujar@urjc.es>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 *          Do not distribute without further notice.
 */

#ifndef __SIMIL__LOADINSITUDATA_H__
#define __SIMIL__LOADINSITUDATA_H__

#include "LoadSimData.h"
#include <cone/cone.hpp>
#include <thread>         // std::thread

namespace simil
{
  class LoadInsituData : public LoadSimData
  {
  public:
      LoadInsituData();
      ~LoadInsituData();
    virtual SimulationData*
      LoadSimulationData( const std::string& filePath_,
                          const std::string& target = "" ) override;
    /*virtual DataSet* LoadNetwork( const std::string& filePath_,
                                  const std::string& target = "" ) override;*/

  protected:
     void SpikeDetectorCB(const nesci::consumer::SpikeDetectorDataView& _spikes);

     void CBloop();
     //void NetworkDataCB(const
    // nesci::consumer::SetNestMultimeterDataView& _network); void
    // UnkwonDataCB(const conduit::Node& _unkwon);
    std::thread looper;
    //DataSet* _dataset;
    SimulationData* _simulationdata;
    cone::Cone*  _cone;
    bool waitForData;
  };

} // namespace simil

#endif /* __SIMIL__LOADINSITUDATA_H__ */
