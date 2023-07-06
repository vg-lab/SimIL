/*
 * Copyright (c) 2015-2020 VG-Lab/URJC.
 *
 * Authors: Sergio E. Galindo <sergio.galindo@urjc.es>
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
#include <cassert>

int main( int argc, char** argv )
{

  if( argc < 3 )
  {
    std::cout << "Usage: " << argv[0] << " network_file.csv activity_file.csv" << std::endl;
    return -1;
  }

  std::string networkFile = argv[ 1 ];
  std::string activityFile = argv[ 2 ];

  simil::CSVNetwork network( networkFile, ',' );
  network.load( );

  const auto gids = network.getGIDs( );
  std::cout << "gids size: " << gids.size( ) << std::endl;

  const auto positions = network.getComposedPositions( );
  assert(gids.size() == positions.size());

  auto pit = positions.cbegin();
  for(auto it = gids.cbegin(); it != gids.cend(); ++it,++pit)
  {
    std::cout << "gid: " << *it << ", pos: " << *pit << std::endl;
  }

  simil::CSVSpikes activity( network, activityFile );
  activity.load( );

  const auto spikes = activity.spikes( );

  for( auto spike : spikes )
    std::cout << "gid: " << spike.first << ", time: " << spike.second << std::endl;
}
