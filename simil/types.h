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

#include <vmmlib/vmmlib.h>

namespace simil
{
  typedef std::set< uint32_t > TGIDSet;
  typedef std::vector< vmml::Vector3f > TPosVect;
  typedef std::multimap< float, uint32_t > TSpikes;

  typedef std::pair< float, float > TimeFrame;
  typedef std::vector< uint32_t > GIDVec;
  typedef std::vector< TimeFrame > TimeFrameVec;

  typedef std::map< std::string, GIDVec > SubsetMap;
  typedef std::map< std::string, TimeFrameVec > TimeFrameMap;

  typedef SubsetMap::const_iterator GIDMapCIt;
  typedef TimeFrameMap::const_iterator TimeFrameMapCIt;

  typedef std::pair< GIDMapCIt, GIDMapCIt > GIDMapRange;
  typedef std::pair< TimeFrameMapCIt, TimeFrameMapCIt > TimeFrameRange;

  typedef enum
  {
    TSimNetwork = 0,
    TSimSpikes,
    TSimVoltages
  } TSimulationType;

  typedef enum
  {
    TBlueConfig = 0,
    THDF5
  } TDataType;
}



#endif /* __SIMIL_TYPES_H__ */
