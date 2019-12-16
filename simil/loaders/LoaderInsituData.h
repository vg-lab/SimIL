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

#include "LoaderSimData.h"
#include <cone/cone.hpp>
#include <thread>

namespace simil
{
  class LoaderInsituData : public LoaderSimData
  {
  public:
    LoaderInsituData( );
    ~LoaderInsituData( );
    virtual SimulationData*
      loadSimulationData( const std::string& filePath_,
                          const std::string& target = "" ) override;
    /*virtual DataSet* LoadNetwork( const std::string& filePath_,
                                  const std::string& target = "" ) override;*/

  protected:
    void
      SpikeDetectorCB( const nesci::consumer::SpikeDetectorDataView& _spikes );
    void
      NetworkDataCB( const nesci::consumer::NestMultimeterDataView& _network );

    void CBloop( );

    std::thread _looper;
    SimulationData* _simulationdata;
    cone::Cone* _cone;
    bool _waitForData;
  };

} // namespace simil

#endif /* __SIMIL__LOADINSITUDATA_H__ */
