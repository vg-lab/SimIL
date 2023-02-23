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
#include "SimulationData.h"
#include <memory>
#include <utility>

namespace simil
{
  DataSet::DataSet( std::shared_ptr< SimulationData > simData ,
                    std::shared_ptr< Network > network )
    : _simulationData( std::move( simData ))
    , _network( std::move( network ))
  {
  }

  std::shared_ptr< SimulationData > DataSet::simulationData( ) const
  {
    return _simulationData;
  }

  void DataSet::simulationData( std::shared_ptr< SimulationData > simData )
  {
    _simulationData = std::move( simData );
  }

  std::shared_ptr< Network > DataSet::network( ) const
  {
    return _network;
  }

  void DataSet::network( std::shared_ptr< Network > network )
  {
    _network = std::move( network );
  }

} // namespace simil
