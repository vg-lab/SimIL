/*
 * @file  DataSet.h
 * @brief
 * @author Sergio E. Galindo <sergio.galindo@urjc.es>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 *          Do not distribute without further notice.
 */

#ifndef __SIMIL__DATASET_H__
#define __SIMIL__DATASET_H__

#include "Network.h"
#include "SimulationData.h"

namespace simil
{
  class DataSet
  {
  public:
    DataSet( SimulationData* simData = nullptr, Network* network = nullptr );
    ~DataSet( );

    SimulationData* simulationData( );
    void simulationData( SimulationData* simData );

    Network* network( );
    void network( Network* network );

  protected:
    SimulationData* _simulationData;
    Network* _network;
  };

} // namespace simil

#endif /* __SIMIL__DATASET_H__ */
