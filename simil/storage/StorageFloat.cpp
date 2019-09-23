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
#include "StorageFloat.h"

namespace simil
{


  StorageFloat::StorageFloat(const std::string& name,
          tDataType datatype,
          TSimulationType simType):Storage(name,datatype,simType)
          {}
          
  StorageFloat::~StorageFloat()
  {

  }


size_t StorageFloat::getSize() const { return _values.size(); }
const FloatVec &StorageFloat::values(void) const { return _values; }

FloatVec *StorageFloat::get(void) { return &_values; }

void StorageFloat::setValue(FloatVec values) { _values = values; }




} // namespace simil


