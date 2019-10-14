/*
 * @file  StorageSparse.h
 * @brief
 * @author Sergio E. Galindo <sergio.galindo@urjc.es>
 * @author Aaron Sújar <aaron.sujar@urjc.es>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 *          Do not distribute without further notice.
 */
#ifndef __SIMIL__STORAGESPARSE_H__
#define __SIMIL__STORAGESPARSE_H__

#include "Storage.h"
#include "../Spikes.hpp"

namespace simil
{
class StorageSparse : public Storage
{
public:
  StorageSparse(const std::string &name,
                tDataType datatype,
                TSimulationType simType);

  ~StorageSparse() override;

  size_t getSize() const;

  const Spikes &spikes(void) const;

  Spikes *get(void);

  void setSpikes(Spikes spikes);
  void setSpikes(FloatVec timesteps, GIDVec GIDs );

protected:
  Spikes _spikes;
};

} // namespace simil

#endif /* __SIMIL__STORAGESPARSE_H__ */
