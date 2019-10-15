/*
 * @file	simData.cpp
 * @brief
 * @author Sergio E. Galindo <sergio.galindo@urjc.es>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 *					Do not distribute without further notice.
 */

#include <simil/simil.h>
#include <simil/loaders/LoadblueConfigData.h>
#include <simil/loaders/LoadHDF5Data.h>
#include <iostream>

int main( int argc, char** argv )
{
  if ( argc < 2 )
  {
    std::cerr << "Error: a file must be provided as a parameter." << std::endl;
    return 1;
  }
  std::string simtype = argv[ 1 ];
  std::string path = argv[ 2 ];
  std::string secondaryPath;

  simil::LoadSimData* importer;



  if ( simtype == "-bc" )
  {

    importer = new simil::LoadblueConfigData( );
  }
  else if ( simtype == "-h5" )
  {
    if ( argc < 4 )
    {
      std::cerr << "Error: an activity file must be provided after network file"
                << std::endl;
      return 1;
    }
    importer = new simil::LoadHDF5Data( );

    secondaryPath = argv[ 3 ];
  }
  else
  {
    importer = new simil::LoadHDF5Data( );
    std::cerr << "Error: a file must be provided " << std::endl;
    return 1;
  }

  std::cout << "--------------------------------------" << std::endl;
  std::cout << "Network" << std::endl;
  std::cout << "--------------------------------------" << std::endl;

  simil::SimulationData* simData =
    importer->LoadSimulationData( path, secondaryPath );

  std::cout << "Loaded GIDS: " << simData->gids( ).size( ) << std::endl;
  std::cout << "Loaded positions: " << simData->positions( ).size( )
            << std::endl;

  std::cout << "--------------------------------------" << std::endl;
  std::cout << "Spikes" << std::endl;
  std::cout << "--------------------------------------" << std::endl;

  simil::SpikeData* spkData = dynamic_cast< simil::SpikeData* >( simData );

  if ( spkData == nullptr )
  {
    std::cerr << "Error: this example is only prepared to spike data"
              << std::endl;
    return 1;
  }

  simil::TGIDSet gids = spkData->gids( );

  std::cout << "Loaded GIDS: " << gids.size( ) << std::endl;

  simil::TPosVect positions = spkData->positions( );

  std::cout << "Loaded positions: " << positions.size( ) << std::endl;

  simil::TSpikes spikes = spkData->spikes( );
  float startTime = spkData->startTime( );
  float endTime = spkData->endTime( );

  std::cout << "Loaded spikes: " << spikes.size( ) << std::endl;
  std::cout << "Starting from " << startTime << " to " << endTime << std::endl;

  std::cout << "--------------------------------------" << std::endl;

  return 0;
}
