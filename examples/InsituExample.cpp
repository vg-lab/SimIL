/*
 * @file	simData.cpp
 * @brief
 * @author Sergio E. Galindo <sergio.galindo@urjc.es>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 *					Do not distribute without further notice.
 */

#include <simil/simil.h>
#include <simil/loaders/LoaderInsituData.h>
#include <iostream>

int main( int argc, char** argv )
{
  if ( argc != 2 )
  {
    std::cerr << "USAGE: InsituExample insituConfigFile" << std::endl;
    return 1;
  }

  std::string path = argv[ 1 ];

  simil::LoaderSimData* importer;

  importer = new simil::LoaderInsituData( );

  std::cout << "--------------------------------------" << std::endl;
  std::cout << "Network" << std::endl;
  std::cout << "--------------------------------------" << std::endl;

  simil::SimulationData* simData = importer->loadSimulationData( path );

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

  while ( true )
  {
    std::cout << "Loaded gids: " << spkData->gids( ).size( ) << std::endl;
    std::cout << "Loaded spikes: " << spkData->spikes( ).size( ) << std::endl;
    std::this_thread::sleep_for( std::chrono::milliseconds( 3000 ) );
  }

  return 0;
}
