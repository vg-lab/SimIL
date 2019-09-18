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



namespace simil
{
class StorageFloat : public Storage
{
public:
  StorageFloat(const std::string& name,
          tDataType datatype,
          TSimulationType simType);

  ~StorageFloat() override;

  size_t getSize() const ;

  const FloatVec &values(void) const;

  FloatVec *get(void);

  void setValue(FloatVec values);

  protected:
  
  FloatVec _values;

};

} // namespace simil



#endif /* __SIMIL__STORAGESPARSE_H__ */
