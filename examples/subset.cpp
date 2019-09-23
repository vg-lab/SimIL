/*
 * @file  subset.cpp
 * @brief
 * @author Sergio E. Galindo <sergio.galindo@urjc.es>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 *          Do not distribute without further notice.
 */

#include <simil/simil.h>

using namespace simil;

int main( int argc, char** argv )
{

  if( argc < 2 )
    exit( 0 );

  std::string filePath = argv[ 1 ];

  SubsetEventManager sm;
  sm.loadJSON( filePath );

}
