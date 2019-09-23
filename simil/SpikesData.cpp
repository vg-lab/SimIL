/*
 * @file  SimulationData.cpp
 * @brief
 * @author Sergio E. Galindo <sergio.galindo@urjc.es>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 *          Do not distribute without further notice.
 */

#include "SpikesData.h"

#include "H5Activity.h"
#include "loaders/LoadblueConfigData.h"
#include "loaders/LoadHDF5Data.h"
#include "storage/StorageSparse.h"

namespace simil
{



SpikeData::SpikeData(const std::string &filePath_, TDataType dataType,
                     const std::string &report)
    
{
   simil::LoadSimData* importer;

    switch( dataType )
    {
      case TDataType::TBlueConfig:
      {
        importer = new simil::LoadblueConfigData();
        break;
      }
      case TDataType::THDF5:
      {
        importer = new simil::LoadHDF5Data();
        //_simData = new SimulationData( networkPath_, dataType );
        break;
      }
      

      default:
        break;
    }

  _simulationType = simil::TSimSpikes;

  simil::SimulationData* simData = importer->LoadSimulationData(filePath_,report);



    _startTime = simData->startTime( );
    _endTime = simData->endTime( );

    
    simil::StorageList storageList = simData->getStorage("Spikes");

    for(auto storage: storageList)
    {
      simil::StorageSparse* storageSparse =
       dynamic_cast<simil::StorageSparse*>(storage);
      if (storageSparse != NULL)
      {
        
        _spikes = storageSparse->spikes();
        
      }
    }
}

void SpikeData::reduceDataToGIDS(void)
{
  std::cout << "Before: " << _spikes.size() << std::endl;
  TSpikes aux;
  aux.reserve(_spikes.size());
  for (auto spike : _spikes)
    if (_gids.find(spike.second) != _gids.end())
      aux.push_back(spike);

  aux.shrink_to_fit();

  _spikes = Spikes(aux);

  std::cout << "After: " << _spikes.size() << std::endl;
}

const Spikes &SpikeData::spikes(void) const
{
  return _spikes;
}

SpikeData *SpikeData::get(void)
{
  return this;
}

} // namespace simil
