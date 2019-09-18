/*
 * @file  SimulationData.h
 * @brief
 * @author Sergio E. Galindo <sergio.galindo@urjc.es>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 *          Do not distribute without further notice.
 */

#ifndef __SIMIL__SIMULATIONDATA_H__
#define __SIMIL__SIMULATIONDATA_H__

#if defined(__GNUC__) || defined(__clang__)
#define DEPRECATED __attribute__((deprecated))
#elif defined(_MSC_VER)
#define DEPRECATED __declspec(deprecated)
#else
#pragma message("WARNING: You need to implement DEPRECATED for this compiler")
#define DEPRECATED
#endif

#ifdef SIMIL_USE_BRION
#include <brion/brion.h>
#include <brain/brain.h>
#endif

#include <H5Cpp.h>

#include "types.h"
#include "H5Network.h"
#include "SubsetEventManager.h"
#include "Spikes.hpp"
#include "storage/Storage.h"

namespace simil
{
class SimulationData
{
public:
  SimulationData(const std::string &filePath,
                 TDataType dataType,
                 const std::string &target = "");
  virtual ~SimulationData(void);

  /******************
    We will do this deprecated
    ****************/
  const TGIDSet &gids(void) const;

  GIDVec gidsVec(void) const;

  const TPosVect &positions(void) const;

  SubsetEventManager *subsetsEvents(void);

  TSimulationType simulationType(void) const;

  virtual SimulationData *get(void);

  /******************
   Until here We will do this deprecated
    ****************/

  void setStartTime(float startTime);
  void setEndTime(float endTime);
  virtual float startTime(void) const;
  virtual float endTime(void) const;


  void addStorage (Storage * newStorage);
  StorageList getStorage (const std::string& name);
  StorageList getStorage (tDataType datatype);
  StorageList getStorage (TSimulationType simtype);

protected:

TSimulationType _simulationType;

#ifdef SIMIL_USE_BRION
  brion::BlueConfig *_blueConfig;
#endif
  H5Network *_h5Network;

  std::string filePath;

  float _startTime;
  float _endTime;
  TGIDSet _gids;

  TPosVect _positions;

  


///WE WILL DO DEPRECATED
  simil::SubsetEventManager _subsetEventManager;

  //    simil::SubsetMap _subsets;


///TILL HERE

  ///NEW atributes
  StorageList _storage;
};

class SpikeData : public SimulationData
{
public:
  SpikeData(const std::string &filePath, TDataType dataType,
            const std::string &report = "");

  const Spikes &spikes(void) const;

  SpikeData *get(void);

  void reduceDataToGIDS(void);

protected:
  Spikes _spikes;
};

class VoltageData : public SimulationData
{

  VoltageData(const std::string &filePath, TDataType dataType,
              const std::string &report = "");
};

} // namespace simil

#endif /* __SIMIL__SIMULATIONDATA_H__ */
