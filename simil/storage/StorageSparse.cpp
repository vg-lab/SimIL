/*
 * @file  StorageSparse.cpp
 * @brief
 * @author Sergio E. Galindo <sergio.galindo@urjc.es>
 * @author Aaron Sújar <aaron.sujar@urjc.es>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 *          Do not distribute without further notice.
 */

#include "../types.h"
#include "StorageSparse.h"

namespace simil
{

StorageSparse::StorageSparse(const std::string &name,
                             tDataType datatype,
                             TSimulationType simType) 
                             : Storage(name, datatype, simType)
{
}
StorageSparse::~StorageSparse()
{
}

size_t StorageSparse::getSize() const
{
  return _spikes.size();
}

const Spikes &StorageSparse::spikes(void) const
{
  return _spikes;
}

Spikes *StorageSparse::get(void)
{
  return &_spikes;
}

void StorageSparse::setSpikes(Spikes spikes)
{
  _spikes = spikes;
}

} // namespace simil
