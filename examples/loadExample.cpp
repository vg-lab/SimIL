/*
 * @file	simData.cpp
 * @brief
 * @author Sergio E. Galindo <sergio.galindo@urjc.es>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 *					Do not distribute without further notice.
 */

#include <simil/simil.h>


int main( int argc, char** argv )
{

  if( argc < 2 )
  {
    std::cerr << "Error: a file must be provided as a parameter." << std::endl;
    return  1 ;
  }
  std::string simtype = argv[ 1 ];
  std::string path = argv[ 2 ];
  std::string secondaryPath;


  simil::TDataType dataType( simil::TDataType::THDF5 );

  if( simtype == "-bc")
    dataType = simil::TDataType::TBlueConfig;
  else if ( simtype == "-h5" )
  {
    if( argc < 4)
    {
      std::cerr << "Error: an activity file must be provided after network file" << std::endl;
      return 1;
    }

    secondaryPath = argv[ 3 ];
  }

  std::cout << "--------------------------------------" << std::endl;
  std::cout << "Network" << std::endl;
  std::cout << "--------------------------------------" << std::endl;



  {
    simil::SimulationData simData( path, dataType );

    simil::TGIDSet gids = simData.gids( );

    std::cout << "Loaded GIDS: " << gids.size( ) << std::endl;

    simil::TPosVect positions = simData.positions( );

    std::cout << "Loaded positions: " << positions.size( ) << std::endl;
  }

  std::cout << "--------------------------------------" << std::endl;
  std::cout << "Spikes" << std::endl;
  std::cout << "--------------------------------------" << std::endl;

  if(  dataType == simil::TDataType::TBlueConfig || !secondaryPath.empty( ))
  {
    simil::SpikeData simData( path, dataType, secondaryPath );

    simil::TGIDSet gids = simData.gids( );

    std::cout << "Loaded GIDS: " << gids.size( ) << std::endl;

    simil::TPosVect positions = simData.positions( );

    std::cout << "Loaded positions: " << positions.size( ) << std::endl;

    simil::TSpikes spikes = simData.spikes( );

    float startTime = simData.startTime( );
    float endTime = simData.endTime( );

    std::cout << "Loaded spikes: " << spikes.size( ) << std::endl;
    std::cout << "Starting from " << startTime
              << " to " << endTime << std::endl;
  }

  std::cout << "--------------------------------------" << std::endl;

  return 0;
}

