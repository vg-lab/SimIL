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


SimulationData::~SimulationData(void)
{
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
