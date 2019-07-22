/*
 * @file	types.h
 * @brief
 * @author Sergio E. Galindo <sergio.galindo@urjc.es> 
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 *					Do not distribute without further notice.
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
    TDataUndefined
  } TDataType;

}



#endif /* __SIMIL_TYPES_H__ */
