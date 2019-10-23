/*
 * @file  LoadblueConfigData.h
 * @brief
 * @author Aaron Sújar <aaron.sujar@urjc.es>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 *          Do not distribute without further notice.
 */


#include "LoadblueConfigData.h"
//#include "../storage/StorageSparse.h"

namespace simil
{

LoadblueConfigData::LoadblueConfigData()
    : LoadSimData()
    ,_blueConfig(nullptr)
{
}
LoadblueConfigData::~LoadblueConfigData()
{
  if (_blueConfig != nullptr)
    delete _blueConfig;
}

/*DataSet *LoadblueConfigData::LoadNetwork(const std::string &filePath_,
                                         const std::string &target)
{
  DataSet *dataset = new DataSet(filePath_, TBlueConfig, target);

  if (_blueConfig == nullptr)
  {
    _blueConfig = new brion::BlueConfig(filePath_);
  }

  brion::Targets targets = _blueConfig->getTargets();

  brain::Circuit *circuit = new brain::Circuit(*_blueConfig);

  if (!target.empty())
    dataset->setGids(brion::Target::parse(targets, target));
  else
    dataset->setGids(circuit->getGIDs());

  dataset->setPositions(circuit->getPositions(dataset->gids()));

  delete circuit;

  return dataset;
}*/

SimulationData *LoadblueConfigData::LoadSimulationData(
                                    const std::string &filePath_,
                                    const std::string & target)
{
  SpikeData *simulationdata = new SpikeData();

  if (_blueConfig == nullptr)
  {
    _blueConfig = new brion::BlueConfig(filePath_);
  }

  brion::Targets targets = _blueConfig->getTargets();

  brain::Circuit *circuit = new brain::Circuit(*_blueConfig);

  if (!target.empty())
    simulationdata->setGids(brion::Target::parse(targets, target));
  else
    simulationdata->setGids(circuit->getGIDs());

  simulationdata->setPositions(circuit->getPositions(circuit->getGIDs()));


  brain::SpikeReportReader spikeReport(_blueConfig->getSpikeSource());
  simulationdata->setSimulationType(TSimSpikes);
  simulationdata->setSpikes(spikeReport.getSpikes( 0, spikeReport.getEndTime( ) ));
  simulationdata->setStartTime(0.0f);
  simulationdata->setEndTime(spikeReport.getEndTime());

  /*StorageSparse *newStorage = new StorageSparse("Spikes",
                                                tTYPE_UINT,
                                                TSimSpikes);*/

  /*newStorage->setSpikes(spikeReport.getSpikes(0, spikeReport.getEndTime()));

  simulationdata->addStorage(newStorage);*/

  return simulationdata;
}

} // namespace simil


