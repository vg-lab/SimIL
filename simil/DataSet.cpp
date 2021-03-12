/*
 * Copyright (c) 2015-2020 VG-Lab/URJC.
 *
 * Authors: Sergio E. Galindo <sergio.galindo@urjc.es>
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
