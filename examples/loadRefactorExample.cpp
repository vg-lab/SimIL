/*
 * Copyright (c) 2015-2020 GMRV/URJC.
 *
 * Authors: Aaron Sujar <aaron.sujar@urjc.es>
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

#include <simil/simil.h>
#ifdef SIMIL_USE_BRION
#include <simil/loaders/LoaderBlueConfigData.h>
#endif
#include <simil/loaders/LoaderHDF5Data.h>
#include <simil/loaders/LoaderCSVData.h>
#include <iostream>

void usage()
{
    std::cerr << "USAGE: similRefactorLoadExample -bc|-h5|-csv file [target]"
              << std::endl;
#ifdef SIMIL_USE_BRION
    std::cerr << "-bc blueconfig loader" << std::endl;
#else
    std::cerr << "-bc NOT available blueconfig loader"
                 ", consider compile again with BRION support"
              << std::endl;
#endif
    std::cerr << "-h5 HDF5 loader " << std::endl;
    std::cerr << "-csv CSV loader " << std::endl;

}


int main( int argc, char** argv )
{
  if ( argc < 2 )
  {
    usage();
    return 1;
  }
  std::string simtype = argv[ 1 ];
  std::string path = argv[ 2 ];
  std::string secondaryPath;

  simil::LoaderSimData* importer;

#ifdef SIMIL_USE_BRION
  if ( simtype == "-bc" )
  {
    importer = new simil::LoaderBlueConfigData( );
  }
  else
#endif
    if ( argc < 4 )
  {
    std::cerr << "Error: an activity file must be provided after network file"
              << std::endl;
    return 1;
  }
  else
  {
    secondaryPath = argv[ 3 ];
    if ( simtype == "-csv" )
    {
      importer = new simil::LoaderCSVData( );
    }
    else if ( simtype == "-h5" )
    {
      importer = new simil::LoaderHDF5Data( );
    }
    else
    {
      importer = new simil::LoaderHDF5Data( );
      usage();
      return 1;
    }
  }

  std::cout << "--------------------------------------" << std::endl;
  std::cout << "Network" << std::endl;
  std::cout << "--------------------------------------" << std::endl;



  simil::Network * netData =
          importer->loadNetwork( path, secondaryPath );

  std::cout << "Loaded GIDS: " << netData->gids( ).size( ) << std::endl;
  std::cout << "Loaded positions: " << netData->positions( ).size( )
            << std::endl;

  std::cout << "--------------------------------------" << std::endl;
  std::cout << "Spikes" << std::endl;
  std::cout << "--------------------------------------" << std::endl;

  simil::SimulationData* simData =
    importer->loadSimulationData( path, secondaryPath );

  simil::SpikeData* spkData = dynamic_cast< simil::SpikeData* >( simData );

  if ( spkData == nullptr )
  {
    std::cerr << "Error: this example is only prepared to spike data"
              << std::endl;
    return 1;
  }

  simil::TGIDSet gids = netData->gids( );

  std::cout << "Loaded GIDS: " << gids.size( ) << std::endl;

  simil::TSpikes spikes = spkData->spikes( );
  float startTime = spkData->startTime( );
  float endTime = spkData->endTime( );

  std::cout << "Loaded spikes: " << spikes.size( ) << std::endl;
  std::cout << "Starting from " << startTime << " to " << endTime << std::endl;

  std::cout << "--------------------------------------" << std::endl;

  return 0;
}
