/*
 * @file  StorageSparse.cpp
 * @brief
 * @author Sergio E. Galindo <sergio.galindo@urjc.es>
 * @author Aaron Sújar <aaron.sujar@urjc.es>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 *          Do not distribute without further notice.
 */

#include "StorageInt.h"
#include "../types.h"

namespace simil
{

StorageInt::StorageInt(const std::string &name,
                       tDataType datatype,
                       TSimulationType simType)
                      : Storage(name, datatype, simType) {}

StorageInt::~StorageInt() {}

size_t StorageInt::getSize() const { return _values.size(); }
const IntVec &StorageInt::values(void) const { return _values; }

IntVec *StorageInt::get(void) { return &_values; }

void StorageInt::setValue(IntVec values) { _values = values; }

} // namespace simil
