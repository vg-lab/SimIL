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

#include "LoaderCSVData.h"



namespace simil
{
  LoaderCSVData::LoaderCSVData( )
  : LoaderSimData( )
  , _csvNetwork( nullptr )
  , _csvActivity( nullptr )
  { }

  LoaderCSVData::~LoaderCSVData( )
  {
    if ( _csvNetwork != nullptr )
      delete _csvNetwork;

    if ( _csvActivity != nullptr )
      delete _csvActivity;
  }

  Network* LoaderCSVData::loadNetwork( const std::string& filePath_,
                                      const std::string&  )
  {
    Network* _network = new Network( );

    if ( _csvNetwork == nullptr )
    {
      _csvNetwork = new CSVNetwork( filePath_ );
      _csvNetwork->load( );

    }
    _network->setDataType(TCSV);

    _network->setGids( _csvNetwork->getGIDs( ) );

    _network->setPositions( _csvNetwork->getComposedPositions( ) );

    return _network;
  }

  SimulationData*
    LoaderCSVData::loadSimulationData( const std::string& ,
                                       const std::string& activityFile )
  {
    SpikeData* simulationdata = new SpikeData( );


    if ( _csvActivity == nullptr )
    {
      _csvActivity = new CSVSpikes( *_csvNetwork, activityFile, ',', false );
      _csvActivity->load( );
    }

    CSVSpikes* _csvSpikes = dynamic_cast< CSVSpikes* >( _csvActivity );

    simulationdata->setSimulationType( TSimSpikes );
    simulationdata->setSpikes( _csvSpikes->spikes( ) );
    simulationdata->setStartTime( _csvSpikes->startTime( ) );
    simulationdata->setEndTime( _csvSpikes->endTime( ) );

    return simulationdata;
  }

} // namespace simil
