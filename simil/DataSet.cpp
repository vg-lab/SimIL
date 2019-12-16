/*
 * @file  DataSet.cpp
 * @brief
 * @author Sergio E. Galindo <sergio.galindo@urjc.es>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 *          Do not distribute without further notice.
 */

#include "DataSet.h"

namespace simil
{
  DataSet::DataSet( SimulationData* simData, Network* network )
    : _simulationData( simData )
    , _network( network )
  {
  }
  DataSet::~DataSet( )
  {
    if ( _simulationData != nullptr )
      delete _simulationData;
    if ( _network != nullptr )
      delete _network;
  }

  SimulationData* DataSet::simulationData( )
  {
    return _simulationData;
  }
  void DataSet::simulationData( SimulationData* simData )
  {
    _simulationData = simData;
  }

  Network* DataSet::network( )
  {
    return _network;
  }
  void DataSet::network( Network* network )
  {
    _network = network;
  }

} // namespace simil
