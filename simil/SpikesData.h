/*
 * @file  SimulationData.h
 * @brief
 * @author Sergio E. Galindo <sergio.galindo@urjc.es>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 *          Do not distribute without further notice.
 */

#ifndef __SIMIL__SPIKESDATA_H__
#define __SIMIL__SPIKESDATA_H__



#ifdef SIMIL_USE_BRION
#include <brion/brion.h>
#include <brain/brain.h>
#endif

#include <H5Cpp.h>

#include "types.h"
#include "Spikes.hpp"
#include "SimulationData.h"

namespace simil
{

class SpikeData : public SimulationData
{
public:
DEPRECATED
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

#endif /* __SIMIL__SPIKESDATA_H__ */
