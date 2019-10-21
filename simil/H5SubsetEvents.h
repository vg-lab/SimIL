/*
 * @file	H5SubsetEvents.h
 * @brief
 * @author Sergio E. Galindo <sergio.galindo@urjc.es> 
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 *					Do not distribute without further notice.
 */

#ifndef __SIMIL_H5SUBSETEVENTS__
#define __SIMIL_H5SUBSETEVENTS__

#include <H5Cpp.h>

#include "types.h"

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

  class H5SubsetEvents
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
