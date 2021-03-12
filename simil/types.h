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

#ifndef __SIMIL_TYPES_H__
#define __SIMIL_TYPES_H__

#include <vector>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>

#include <vmmlib/vmmlib.h>

namespace simil
{
  class Spikes;

  typedef std::set< uint32_t > TGIDSet;
  typedef std::unordered_set< uint32_t > TGIDUSet;
  typedef std::vector< uint32_t > TGIDVect;
  typedef std::vector< vmml::Vector3f > TPosVect;

  typedef std::pair< float, uint32_t > Spike;
  typedef std::vector< Spike > TSpikes;

  typedef std::pair< float, float > Event;
  typedef std::vector< uint32_t > GIDVec;
  typedef std::vector< Event > EventVec;

  typedef std::map< std::string, GIDVec > SubsetMap;
  typedef std::map< std::string, EventVec > EventMap;

  typedef SubsetMap::const_iterator SubsetMapCIt;
  typedef EventMap::const_iterator EventMapCIt;

  typedef std::pair< SubsetMapCIt, SubsetMapCIt > SubsetMapRange;
  typedef std::pair< EventMapCIt, EventMapCIt > EventRange;

  typedef enum
  {
    TSimNetwork = 0,
    TSimSpikes,
    TSimVoltages
  } TSimulationType;

  typedef enum
  {
    TBlueConfig = 0,
    THDF5,
    TCSV,
    TREST,
    TCONE,
    TDataUndefined
  } TDataType;

}



#endif /* __SIMIL_TYPES_H__ */
