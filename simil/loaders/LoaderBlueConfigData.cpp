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

#include "LoaderBlueConfigData.h"


namespace simil
{
  LoaderBlueConfigData::LoaderBlueConfigData( )
    : LoaderSimData( )
    , _blueConfig( nullptr )
  { }

  LoaderBlueConfigData::~LoaderBlueConfigData( )
  {
    delete _blueConfig;
  }

  std::unique_ptr< Network >
  LoaderBlueConfigData::loadNetwork( const std::string& filePath_ ,
                                     const std::string& aux )
  {
    auto _network = std::unique_ptr< Network >( new Network( ));

    if ( _blueConfig == nullptr )
    {
      _blueConfig = new brion::BlueConfig( filePath_ );
    }

    _network->setDataType( TBlueConfig );

    brion::Targets targets = _blueConfig->getTargets( );

    brain::Circuit* circuit = new brain::Circuit( *_blueConfig );

    if ( !aux.empty( ))
      _network->setGids( brion::Target::parse( targets , aux ));
    else
      _network->setGids( circuit->getGIDs( ));

    _network->setPositions( circuit->getPositions( _network->gids( )));

    delete circuit;

    return _network;
  }

  std::unique_ptr< SimulationData >
  LoaderBlueConfigData::loadSimulationData( const std::string& filePath_ ,
                                            const std::string& )
  {
    auto simulationdata = std::unique_ptr< SpikeData >( new SpikeData( ));

    if ( _blueConfig == nullptr )
    {
      _blueConfig = new brion::BlueConfig( filePath_ );
    }

    brain::SpikeReportReader spikeReport( _blueConfig->getSpikeSource( ));
    simulationdata->setSimulationType( TSimSpikes );
    simulationdata->setSpikes(
      spikeReport.getSpikes( 0 , spikeReport.getEndTime( )));
    simulationdata->setStartTime( 0.0f );
    simulationdata->setEndTime( spikeReport.getEndTime( ));

    return simulationdata;
  }

} // namespace simil
