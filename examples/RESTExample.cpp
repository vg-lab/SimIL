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

#include <simil/simil.h>
#include <simil/loaders/LoaderRestData.h>
#include <iostream>

int main( int , char** )
{
  simil::LoaderSimData* importer;

  importer = new simil::LoaderRestData( );

  std::cout << "--------------------------------------" << std::endl;
  std::cout << "Network" << std::endl;
  std::cout << "--------------------------------------" << std::endl;

  std::shared_ptr< simil::SimulationData > simData =
    importer->loadSimulationData( "localhost" , "8080" );

  auto netData = importer->loadNetwork( "localhost" , "8080" );

  std::cout << "Loaded GIDS: " << netData->gids( ).size( ) << std::endl;
  std::cout << "Loaded positions: " << netData->positions( ).size( )
            << std::endl;

  std::cout << "--------------------------------------" << std::endl;
  std::cout << "Spikes" << std::endl;
  std::cout << "--------------------------------------" << std::endl;

  auto spkData = std::dynamic_pointer_cast< simil::SpikeData >( simData );

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
    std::this_thread::sleep_for( std::chrono::milliseconds( 3000 ));
  }

  return 0;
}
