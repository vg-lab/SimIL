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

  /******************
    We will do this deprecated
    ****************/
   DEPRECATED
 SimulationData(const std::string &filePath,
                 TDataType dataType,
                 const std::string &target = "");

  DEPRECATED const TGIDSet &gids(void) const;

  DEPRECATED GIDVec gidsVec(void) const;

  DEPRECATED const TPosVect &positions(void) const;

  DEPRECATED SubsetEventManager *subsetsEvents(void);

  DEPRECATED TSimulationType simulationType(void) const;

  DEPRECATED virtual SimulationData *get(void);

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
//We will do this deprecated
TSimulationType _simulationType;

#ifdef SIMIL_USE_BRION
  brion::BlueConfig *_blueConfig;
#endif
  H5Network *_h5Network;

  std::string filePath;
// Unitl Here
  float _startTime;
  float _endTime;

///WE WILL DO DEPRECATED
  TGIDSet _gids;

  TPosVect _positions;

  


  simil::SubsetEventManager _subsetEventManager;

  //    simil::SubsetMap _subsets;


///TILL HERE

  ///NEW atributes
  StorageList _storage;
};



} // namespace simil

#endif /* __SIMIL__SIMULATIONDATA_H__ */
