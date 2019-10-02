/*
 * @file  Storage.h
 * @brief
 * @author Sergio E. Galindo <sergio.galindo@urjc.es>
 * @author Aaron Sújar <aaron.sujar@urjc.es>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 *          Do not distribute without further notice.
 */
#ifndef __SIMIL__STORAGE_H__
#define __SIMIL__STORAGE_H__

#include "../types.h"

namespace simil
{
class Storage
{
public:
  Storage(const std::string& name,
          tDataType datatype,
          TSimulationType simType):
          _name(name),
          _type(datatype),
          _simulationType(simType)
          {}

  virtual ~Storage()=0;

  virtual size_t getSize() const = 0;

  std::string _name;
  tDataType _type;
  TSimulationType _simulationType;

protected:
};

inline Storage::~Storage()
{
    std::cout << "Pure virtual destructor is called";
}

} // namespace simil



#endif /* __SIMIL__STORAGE_H__ */
