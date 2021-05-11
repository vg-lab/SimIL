/*
 * Copyright (c) 2015-2020 VG-Lab/URJC.
 *
 * Authors: Sergio E. Galindo <sergio.galindo@urjc.es>
 * Authors: Pablo Toharia <pablo.toharia@upm.es>
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
#include <iostream>
#include <fstream>
#include <locale>

struct dotSeparator: std::numpunct<char>
{
    char do_decimal_point() const { return '.'; }
};

int main( int argc, char** argv )
{

  if( argc < 4 )
  {
    std::cerr << "Usage: " << argv[0] << "in_blueconfig out_structure out_spikes"
              << std::endl << std::endl;
    return  1 ;
  }
  std::string path = argv[ 1 ];
  const auto dataType = simil::TDataType::TBlueConfig;

  std::cout << "--------------------------------------" << std::endl;
  std::cout << "Network" << std::endl;
  std::cout << "--------------------------------------" << std::endl;
  {
    simil::SimulationData simData( path, dataType );
    const auto gids = simData.gidsVec( );
    std::cout << "Loaded GIDS: " << gids.size( ) << std::endl;
    simil::TPosVect positions = simData.positions( );
    std::cout << "Loaded positions: " << positions.size( ) << std::endl;
    
    std::ofstream structure;
    structure.open(argv[2]);

    if(structure.fail())
    {
      std::cerr << "Unable to create/open structure file: " << argv[2] << std::endl;
      exit(-1);
    }

    auto loc = std::locale(structure.getloc(), new dotSeparator());
    structure.imbue(loc);

    for ( unsigned int i = 0; i < positions.size( ); ++i )
    {
      structure << gids.at(i) << ","
                << positions[i].x( ) << ","
                << positions[i].y( ) << ","
                << positions[i].z( ) << std::endl;
    }

    structure.close();
  }
 
  std::cout << "--------------------------------------" << std::endl;
  std::cout << "Spikes" << std::endl;
  std::cout << "--------------------------------------" << std::endl;

  {
    simil::SpikeData simData( path, dataType );
    simil::TGIDSet gids = simData.gids( );
    std::cout << "Loaded GIDS: " << gids.size( ) << std::endl;
    simil::TPosVect positions = simData.positions( );
    std::cout << "Loaded positions: " << positions.size( ) << std::endl;
    simil::TSpikes spikes = simData.spikes( );

    const float startTime = simData.startTime( );
    const float endTime = simData.endTime( );

    std::cout << "Loaded spikes: " << spikes.size( ) << std::endl;
    std::cout << "Starting from " << startTime
              << " to " << endTime << std::endl;

    std::ofstream spikesFile;
    spikesFile.open(argv[3]);

    if(spikesFile.fail())
    {
      std::cerr << "Unable to create/open spikes file: " << argv[3] << std::endl;
      exit(-1);
    }

    auto loc = std::locale(spikesFile.getloc(), new dotSeparator());
    spikesFile.imbue(loc);

    auto writeSpike = [&spikesFile](const simil::Spike &spike)
    {
      spikesFile << spike.second << "," << spike.first << std::endl;
    };
    std::for_each(spikes.cbegin(), spikes.cend(), writeSpike);

    spikesFile.close( );
  }

  std::cout << "--------------------------------------" << std::endl;

  return 0;
}

