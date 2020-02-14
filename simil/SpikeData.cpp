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
#include "SpikeData.h"

#include "loaders/aux/H5Activity.h"

namespace simil
{
SpikeData::SpikeData()
: SimulationData()
{
    _simulationType = simil::TSimSpikes;
}
#ifdef SIMIL_USE_BRION
  const brion::BlueConfig* SimulationData::blueConfig( void ) const
  {
    return _blueConfig;
  }

  const std::string& SimulationData::target( void ) const
  {
    return _target;
  }

#endif

    SpikeData::SpikeData( const std::string& filePath_, TDataType dataType,
                        const std::string& report )
    : SimulationData( filePath_, dataType, report )
  {
    _simulationType = simil::TSimSpikes;

    switch ( dataType )
    {
      case TBlueConfig:
      {
#ifdef SIMIL_USE_BRION
        if ( _blueConfig )
        {
          brain::SpikeReportReader spikeReport(
          _blueConfig->getSpikeSource( ) );
          _spikes = spikeReport.getSpikes( 0, spikeReport.getEndTime( ) );

          _startTime = 0.0f;
          _endTime = spikeReport.getEndTime( );
        }
#else
        std::cerr << "Error: Brion support not available" << std::endl;
        exit( -1 );
#endif
        break;
      }
      case THDF5:
      {
        if ( report.empty( ) )
        {
          std::cerr << "Error: Activity file path is empty." << std::endl;
        }

        H5Spikes spikeReport( *_h5Network, report );
        spikeReport.Load( );

        _spikes = spikeReport.spikes( );

        _startTime = spikeReport.startTime( );
        _endTime = spikeReport.endTime( );

        break;
      }
      case TCSV:
      {
        CSVSpikes spikeReport( *_csvNetwork, report, ',', false );
        spikeReport.load( );

        _spikes = spikeReport.spikes( );

        _startTime = spikeReport.startTime();
        _endTime = spikeReport.endTime( );

        break;
      }
      default:
        break;
    }
  }

  void SpikeData::setSpikes( Spikes spikes )
  {
    _isDirty=true;
    _spikes = spikes;
  }

  void SpikeData::reduceDataToGIDS( void )
  {
    _isDirty = true;
    std::cout << "Before: " << _spikes.size( ) << std::endl;
    TSpikes aux;
    aux.reserve( _spikes.size( ) );
    for ( auto spike : _spikes )
      if ( _gids.find( spike.second ) != _gids.end( ) )
        aux.push_back( spike );

    aux.shrink_to_fit( );

    _spikes = Spikes( aux );

    std::cout << "After: " << _spikes.size( ) << std::endl;
  }

  const Spikes& SpikeData::spikes( void ) const
  {
    return _spikes;
  }

  void SpikeData::addSpikes(TSpikes & spikes)
  {
    _isDirty = true;
    _spikes.insert(_spikes.end(),spikes.begin(),spikes.end());
    _spikes.rebuildIndex(_spikes.size());
  }

  SpikeData* SpikeData::get( void )
  {
    return this;
  }

} // namespace simil
