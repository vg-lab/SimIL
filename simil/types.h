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
  class DataSet;
  class SimulationData;
  class Storage;
  class StepData;

  typedef std::vector< DataSet > DataSets;
  typedef std::vector< SimulationData* > SimulationDatas;
  typedef std::vector< StepData > StepDatas;


  typedef std::set< uint32_t > TGIDSet;
  typedef std::unordered_set< uint32_t > TGIDUSet;
  typedef std::vector< vmml::Vector3f > TPosVect;
  typedef std::vector< int > TNTypeVect;

  typedef std::vector< Storage* > StorageList;

  typedef std::pair< float, uint32_t > Spike;
  typedef std::vector< Spike > TSpikes;
  typedef std::vector< int > IntVec;
  typedef std::vector< float > FloatVec;

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
    tTYPE_INT = 0,
    tTYPE_UINT,
    tTYPE_FLOAT,
    tTYPE_DOUBLE,
    tTYPE_BOOL,
    tTYPE_VOID
  }tDataType;


  typedef enum
  {
    TBlueConfig = 0,
    THDF5,
    TInSitu
  } TDataType;

  struct CorrelationValues
  {
  public:

    float hit;
    float falseHit;
    float result;

    bool operator==( const CorrelationValues& other ) const
    { return result == other.result; }

    bool operator>( const CorrelationValues& other ) const
    { return result > other.result; }
  };

  typedef std::map< uint32_t, CorrelationValues > TNeuronCorrelationUMap;
  typedef TNeuronCorrelationUMap::const_iterator TNeuronCorrelUMapCIt;
  typedef std::pair< TNeuronCorrelUMapCIt,
                     TNeuronCorrelUMapCIt > TNeuronCorrelationRange;

  struct Correlation
  {
  public:

    std::string subsetName;
    std::string eventName;

    TNeuronCorrelationUMap values;
  };

}



#endif /* __SIMIL_TYPES_H__ */
