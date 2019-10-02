/*
 * @file  StorageSparse.h
 * @brief
 * @author Sergio E. Galindo <sergio.galindo@urjc.es>
 * @author Aaron Sújar <aaron.sujar@urjc.es>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 *          Do not distribute without further notice.
 */
#ifndef __SIMIL__STORAGEINT_H__
#define __SIMIL__STORAGEINT_H__

#include "Storage.h"

namespace simil
{
class StorageInt : public Storage
{
public:
  StorageInt(const std::string &name,
             tDataType datatype,
             TSimulationType simType);

  ~StorageInt() override;

  size_t getSize() const;

  const IntVec &values(void) const;

  IntVec *get(void);

  void setValue(IntVec values);

protected:
  IntVec _values;
};

} // namespace simil

#endif /* __SIMIL__STORAGEINT_H__ */
