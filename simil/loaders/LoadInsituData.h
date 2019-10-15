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
    // void SpikeDetectorCB(const nesci::consumer::SpikeDetectorDataView&
    // _spikes); void NetworkDataCB(const
    // nesci::consumer::SetNestMultimeterDataView& _network); void
    // UnkwonDataCB(const conduit::Node& _unkwon);
    // Cone _cone;
    //DataSet* _dataset;
    SimulationData* _simulationdata;
  };

} // namespace simil

#endif /* __SIMIL__LOADINSITUDATA_H__ */
