/*
 * @file	simData.cpp
 * @brief
 * @author Sergio E. Galindo <sergio.galindo@urjc.es>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 *					Do not distribute without further notice.
 */

#include <simil/simil.h>
#include <simil/loaders/LoaderRestData.h>
#include <iostream>

int main( int , char**  )
{
  simil::LoaderSimData* importer;

  importer = new simil::LoaderRestData( );

  std::cout << "--------------------------------------" << std::endl;
  std::cout << "Network" << std::endl;
  std::cout << "--------------------------------------" << std::endl;

  simil::SimulationData* simData = importer->loadSimulationData("localhost","3000");

  simil::Network* netData = importer->loadNetwork("localhost","3000");

  std::cout << "Loaded GIDS: " << netData->gids( ).size( ) << std::endl;
  std::cout << "Loaded positions: " << netData->positions( ).size( )
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



  simil::TSpikes spikes = spkData->spikes( );
  float startTime = spkData->startTime( );
  float endTime = spkData->endTime( );

  std::cout << "Loaded spikes: " << spikes.size( ) << std::endl;
  std::cout << "Starting from " << startTime << " to " << endTime << std::endl;

  std::cout << "--------------------------------------" << std::endl;

  while ( true )
  {
    std::cout << "Loaded gids: " << netData->gids( ).size( ) << std::endl;
    std::cout << "Loaded spikes: " << spkData->spikes( ).size( ) << std::endl;
    std::this_thread::sleep_for( std::chrono::milliseconds( 3000 ) );
  }

  return 0;
}
