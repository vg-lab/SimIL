/*
 * Copyright (c) 2015-2020 VG-Lab/URJC.
 *
 * Authors: Sergio E. Galindo <sergio.galindo@urjc.es>
 *
 * This file is part of SimIL <https://github.com/vg-lab/SimIL>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#ifndef __SIMIL_H5SUBSETEVENTS__
#define __SIMIL_H5SUBSETEVENTS__

#include <H5Cpp.h>

#include "../../types.h"
#include <simil/api.h>

namespace simil
{
  struct TSubset
  {
  public:
    std::string name;
    GIDVec gids;
  } ;

  struct TTimeFrame
  {
  public:
    std::string name;
    EventVec timeFrames;
  };

  class SIMIL_API H5SubsetEvents
  {
  public:

    H5SubsetEvents( void );

    void Load( const std::string& fileName,
               const std::string& binsName,
               const std::string& matrixName );

    const std::vector< TSubset >& subsets( void ) const;

    const std::vector< TTimeFrame >& timeFrames( void ) const;

    float totalTime( void ) const;

  protected:

    std::vector< TSubset > _subsets;

    std::vector< TTimeFrame > _events;

    float _totalTime;
  };

}


#endif /* __SIMIL_H5SUBSETEVENTS__ */
