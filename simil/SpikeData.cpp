/*
 * @file  SpikeData.cpp
 * @brief
 * @author Sergio E. Galindo <sergio.galindo@urjc.es>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 *          Do not distribute without further notice.
 */

#include "SpikeData.h"

#include "loaders/auxiliar/H5Activity.h"

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
