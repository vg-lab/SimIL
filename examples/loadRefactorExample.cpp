/*
 * @file	simData.cpp
 * @brief
 * @author Sergio E. Galindo <sergio.galindo@urjc.es>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 *					Do not distribute without further notice.
 */

#include <simil/simil.h>
#include <simil/loaders/LoadblueConfigData.h>
#include <simil/loaders/LoadHDF5Data.h>
#include <simil/storage/StorageSparse.h>
#include <iostream>

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

  simil::LoadSimData* importer;

  simil::TDataType dataType( simil::TDataType::THDF5 );

  if( simtype == "-bc")
  {
    dataType = simil::TDataType::TBlueConfig;
    importer = new simil::LoadblueConfigData();

  }
  else if ( simtype == "-h5" )
  {
    if( argc < 4)
    {
      std::cerr << "Error: an activity file must be provided after network file" << std::endl;
      return 1;
    }
    importer = new simil::LoadHDF5Data();

    secondaryPath = argv[ 3 ];
  }
  else{
    importer = new simil::LoadHDF5Data();
    std::cerr << "Error: a file must be provided "<< std::endl;
      return 1;
  }

  std::cout << "--------------------------------------" << std::endl;
  std::cout << "Network" << std::endl;
  std::cout << "--------------------------------------" << std::endl;


  simil::DataSet* simDataSet = importer->LoadNetwork(path,secondaryPath);

  

    std::cout << "Loaded GIDS: " << simDataSet->gids().size( ) << std::endl;
    std::cout << "Loaded positions: " << simDataSet->positions().size( ) << std::endl;
  

  std::cout << "--------------------------------------" << std::endl;
  std::cout << "Spikes" << std::endl;
  std::cout << "--------------------------------------" << std::endl;

  if(  dataType == simil::TDataType::TBlueConfig || !secondaryPath.empty( ))
  {

    simil::SimulationData* simData = importer->LoadSimulationData(path,secondaryPath);

    simDataSet->setSimulationData(simData);

    float startTime = simData->startTime( );
    float endTime = simData->endTime( );

    std::cout << "Looking for spikes" << std::endl;
    simil::StorageList storageList = simData->getStorage("Spikes");

    for(auto storage: storageList)
    {
      simil::StorageSparse* storageSparse =
       dynamic_cast<simil::StorageSparse*>(storage);
      if (storageSparse != NULL)
      {
        
        simil::Spikes* spikes = storageSparse->get();
        std::cout << "Loaded spikes: " << spikes->size( ) << std::endl;
        std::cout << "Starting from " << startTime
              << " to " << endTime << std::endl;
      }
    }

  }

  std::cout << "--------------------------------------" << std::endl;

  return 0;
}

