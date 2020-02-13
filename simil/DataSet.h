/*
 * Copyright (c) 2015-2020 GMRV/URJC.
 *
 * Authors: Sergio E. Galindo <sergio.galindo@urjc.es>
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

#ifndef __SIMIL__DATASET_H__
#define __SIMIL__DATASET_H__

#include "Network.h"
#include "SimulationData.h"
#include "SpikeData.h"

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
