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

  std::cout << "GIDs: ";
  for( auto gid : gids )
    std::cout << " "<< gid;
  std::cout << std::endl;

  simil::TPosVect positions = network.getComposedPositions( );
  std::cout << positions.size( ) << std::endl;

  for( auto position : positions )
    std::cout << position << std::endl;

  simil::CSVSpikes activity( network, activityFile, ',', false );
  activity.load( );

  simil::TSpikes spikes = activity.spikes( );

//  for( auto spike : spikes )
//    std::cout << spike.first << ", " << spike.second << std::endl;
}
