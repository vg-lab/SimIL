/*
 * @file  DataSet.h
 * @brief
 * @author Sergio E. Galindo <sergio.galindo@urjc.es>
 * @author Aaron Sújar <aaron.sujar@urjc.es>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 *          Do not distribute without further notice.
 */

#include "DataSet.h"

namespace simil
{

DataSet::DataSet(const std::string &filePath_,
                 TDataType dataType,
                 const std::string &target)
    : _filePath(filePath_)
    , _simOrigin(dataType)
    , _target(target)
{
}

DataSet::~DataSet(void)
{
}

const TGIDSet &DataSet::gids(void) const
{
  return _gids;
}
void DataSet::setGids(const TGIDSet &gids)
{
  _gids = gids;
}

GIDVec DataSet::gidsVec(void) const
{
  return GIDVec(_gids.begin(), _gids.end());
}

void DataSet::setPositions(TPosVect positions)
{
  _positions = positions;
}

const TPosVect &DataSet::positions(void) const
{
  return _positions;
}

void DataSet::setSubset(SubsetEventManager subsets)
{
  _subsetEventManager = subsets;
}
const SubsetEventManager &DataSet::Subsets()
{
  return _subsetEventManager;
}

const TNTypeVect &DataSet::neurontypes(void) const
{
  return _neurontypes;
}

void DataSet::setSimulationData(SimulationData *simData)
{
  _simulationdata.push_back(simData);
}

SimulationData *DataSet::get(uint index)
{
  if (_simulationdata.size() > index)
    return _simulationdata[index];
  return NULL;
}
} // namespace simil