/*
 * Copyright (c) 2015-2020 VG-Lab/URJC.
 *
 * Authors: Aaron Sujar <aaron.sujar@urjc.es>
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

#include "LoaderHDF5Data.h"

namespace simil
{
  LoaderHDF5Data::LoaderHDF5Data( )
  : LoaderSimData( )
  , _h5Network( nullptr )
  { }

  LoaderHDF5Data::~LoaderHDF5Data( )
  {
    if ( _h5Network != nullptr )
      delete _h5Network;
  }

  Network* LoaderHDF5Data::loadNetwork( const std::string& networkFile,
                                      const std::string&  )
  {
    Network* _network = new Network( );

    if ( _h5Network == nullptr )
    {
      _h5Network = new H5Network( networkFile );
      _h5Network->load( );
    }
    _network->setDataType(THDF5);

    _network->setGids( _h5Network->getGIDs( ) );

    _network->setPositions( _h5Network->getComposedPositions( ) );

    SubsetEventManager subsetEventManager;
    const auto subsetIts = _h5Network->getSubsets( );
    const auto &subsetColors = _h5Network->getSubsetsColors();

    for ( simil::SubsetMapCIt it = subsetIts.first; it != subsetIts.second;
          ++it )
      subsetEventManager.addSubset( it->first, it->second, subsetColors.at(it->first) );

    _network->setSubset( subsetEventManager );

    return _network;
  }

  SimulationData*
    LoaderHDF5Data::loadSimulationData( const std::string& networkFile,
                                        const std::string& activityFile )
  {
    SpikeData* simulationdata = new SpikeData( );

    if ( _h5Network == nullptr )
    {
      _h5Network = new H5Network( networkFile );
      _h5Network->load( );
    }

    simulationdata->setGids( _h5Network->getGIDs( ) );
    simulationdata->setPositions( _h5Network->getComposedPositions( ) );

    simil::H5Spikes spikeReport( *_h5Network, activityFile );
    spikeReport.Load( );

    SubsetEventManager subsetEventManager;
    const auto subsetIts = _h5Network->getSubsets( );
    const auto &subsetColors = _h5Network->getSubsetsColors();

    for ( simil::SubsetMapCIt it = subsetIts.first; it != subsetIts.second;
          ++it )
      subsetEventManager.addSubset( it->first, it->second, subsetColors.at(it->first) );

    simulationdata->setSubset( subsetEventManager );

    /*simil::StorageSparse* newStorage =
      new StorageSparse( "Spikes", tTYPE_UINT, TSimSpikes );*/

    simulationdata->setSimulationType( TSimSpikes );
    simulationdata->setSpikes( spikeReport.spikes( ) );
    simulationdata->setStartTime( spikeReport.startTime( ) );
    simulationdata->setEndTime( spikeReport.endTime( ) );

    // simulationdata->addStorage( newStorage );

    return simulationdata;
  }

} // namespace simil
