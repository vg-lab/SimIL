/*
 * @file  csv.cpp
 * @brief
 * @author Sergio E. Galindo <sergio.galindo@urjc.es>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 *          Do not distribute without further notice.
 */

#include <simil/simil.h>
#include <iostream>

int main( int argc, char** argv )
{

  if( argc < 3 )
    return -1;

  std::string networkFile = argv[ 1 ];
  std::string activityFile = argv[ 2 ];

  simil::CSVNetwork network( networkFile, ',', false );
  network.load( );

  simil::TGIDSet gids = network.getGIDs( );
  std::cout << gids.size( ) << std::endl;

//  for( auto gid : gids )
//    std::cout << gid << std::endl;

  simil::TPosVect positions = network.getComposedPositions( );
  std::cout << positions.size( ) << std::endl;

//  for( auto position : positions )
//    std::cout << position << std::endl;

  simil::CSVSpikes activity( network, activityFile, ',', false );
  activity.load( );

  simil::TSpikes spikes = activity.spikes( );

  for( auto spike : spikes )
    std::cout << spike.first << ", " << spike.second << std::endl;
}
