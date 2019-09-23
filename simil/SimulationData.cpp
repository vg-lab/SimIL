/*
 * @file  SimulationData.cpp
 * @brief
 * @author Sergio E. Galindo <sergio.galindo@urjc.es>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 *          Do not distribute without further notice.
 */

#include "SimulationData.h"

#include "H5Activity.h"

namespace simil
{
  SimulationData::SimulationData()
  : _startTime(0.0f)
  , _endTime(0.0f)
  {

  }

SimulationData::SimulationData(const std::string &filePath_,
                               TDataType dataType,
                               const std::string &target)
    : _simulationType(TSimNetwork)
#ifdef SIMIL_USE_BRION
      ,
      _blueConfig(nullptr)
#endif
      ,
      _h5Network(nullptr), _startTime(0.0f), _endTime(0.0f)
{
  target.size(); // TODO remove this workaround to unused variable error
  switch (dataType)
  {
  case TBlueConfig:
  {
#ifdef SIMIL_USE_BRION
    _blueConfig = new brion::BlueConfig(filePath_);
    brion::Targets targets = _blueConfig->getTargets();

    brain::Circuit *circuit = new brain::Circuit(*_blueConfig);

    if (!target.empty())
      _gids = brion::Target::parse(targets, target);
    else
      _gids = circuit->getGIDs();

    _positions = circuit->getPositions(_gids);

    delete circuit;
#else
    std::cerr << "Error: Brion support not available" << std::endl;
    exit(-1);
#endif
    break;
  }
  case THDF5:
  {
    _h5Network = new H5Network(filePath_);
    _h5Network->load();

    _gids = _h5Network->getGIDs();

    _positions = _h5Network->getComposedPositions();

    auto subsetIts = _h5Network->getSubsets();
    for (simil::SubsetMapCIt it = subsetIts.first; it != subsetIts.second; ++it)
      _subsetEventManager.addSubset(it->first, it->second);

    break;
  }
  default:
    break;
  }
}

SimulationData::~SimulationData(void)
{
}

const TGIDSet &SimulationData::gids(void) const
{
  return _gids;
}

GIDVec SimulationData::gidsVec(void) const
{
  return GIDVec(_gids.begin(), _gids.end());
}

const TPosVect &SimulationData::positions(void) const
{
  return _positions;
}

simil::SubsetEventManager *SimulationData::subsetsEvents(void)
{
  return &_subsetEventManager;
}

TSimulationType SimulationData::simulationType(void) const
{
  return _simulationType;
}

SimulationData *SimulationData::get(void)
{
  return this;
}

void SimulationData::setStartTime(float startTime)
{
  _startTime = startTime;
}
void SimulationData::setEndTime(float endTime)
{
  _endTime = endTime;
}

float SimulationData::startTime(void) const
{
  return _startTime;
}

float SimulationData::endTime(void) const
{
  return _endTime;
}

void SimulationData::addStorage(Storage *newStorage)
{
  _storage.push_back(newStorage);
}
StorageList SimulationData::getStorage(const std::string &name)
{
  StorageList result;
  result.clear();
  for (auto storage : _storage)
  {
      if (name.compare(storage->_name)==0)
        result.push_back(storage);
  }
  return result;
}
StorageList SimulationData::getStorage(tDataType datatype)
{
  StorageList result;
  result.clear();
  for (auto storage : _storage)
  {
      if (storage->_type == datatype)
        result.push_back(storage);
  }
  return result;
}
StorageList SimulationData::getStorage(TSimulationType simtype)
{
  StorageList result;
  result.clear();
  for (auto storage : _storage)
  {
      if (storage->_simulationType == simtype)
        result.push_back(storage);
  }
  return result;
}

} // namespace simil
