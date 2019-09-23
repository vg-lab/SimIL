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



#ifdef SIMIL_USE_BRION
#include <brion/brion.h>
#include <brain/brain.h>
#endif

#include <H5Cpp.h>

#include "types.h"
#include "H5Network.h"
#include "SubsetEventManager.h"

#include "storage/Storage.h"

namespace simil
{
class SimulationData
{
public:

  SimulationData();

  virtual ~SimulationData(void);


  SimulationData *get(void);


  void setStartTime(float startTime);
  void setEndTime(float endTime);
  virtual float startTime(void) const;
  virtual float endTime(void) const;


  void addStorage (Storage * newStorage);
  StorageList getStorage (const std::string& name);
  StorageList getStorage (tDataType datatype);
  StorageList getStorage (TSimulationType simtype);

protected:


  float _startTime;
  float _endTime;

  simil::SubsetEventManager _subsetEventManager;

  StorageList _storage;
};



} // namespace simil

#endif /* __SIMIL__SIMULATIONDATA_H__ */
