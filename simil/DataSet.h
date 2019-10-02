/*
 * @file  DataSet.h
 * @brief
 * @author Sergio E. Galindo <sergio.galindo@urjc.es>
 * @author Aaron Sújar <aaron.sujar@urjc.es>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 *          Do not distribute without further notice.
 */
#ifndef SIMIL_DATASET_H_
#define SIMIL_DATASET_H_

#include <vector>
#include <string>
#include <functional>
#include <unordered_map>

#include <boost/any.hpp>

#include "SimulationData.h"
#include "SubsetEventManager.h"

namespace simil
{
class DataSet
{
public:
  DataSet(const std::string &filePath,
          TDataType dataType,
          const std::string &target = "");
  virtual ~DataSet(void);

  void setGids(const TGIDSet &gids);

  const TGIDSet &gids(void) const;

  GIDVec gidsVec(void) const;

  const TPosVect &positions(void) const;

  void setPositions(TPosVect positions);

  void setSubset(SubsetEventManager subsets);
  const SubsetEventManager &Subsets();

  const TNTypeVect &neurontypes(void) const;

  virtual SimulationData *get(uint index);

  void setSimulationData(SimulationData *simData);

protected:

  std::string _filePath;
  TDataType _simOrigin;
  std::string _target;

  TGIDSet _gids;

  TPosVect _positions;

  TNTypeVect _neurontypes;

  SimulationDatas _simulationdata;

  SubsetEventManager _subsetEventManager;
};

} // namespace simil

#endif /* SIMIL_DATASET_H_ */
