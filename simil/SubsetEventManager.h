/*
 * @file  SubsetEventManager.h
 * @brief
 * @author Sergio E. Galindo <sergio.galindo@urjc.es>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 *          Do not distribute without further notice.
 */
#ifndef __SIMIL_SUBSETEVENTMANAGER__
#define __SIMIL_SUBSETEVENTMANAGER__

#include <map>
#include <vector>

#include <simil/api.h>

#include "types.h"

namespace simil
{

  class SubsetEventManager
  {

  public:

    SIMIL_API
    void loadJSON( const std::string& filePath, bool append = false );

    SIMIL_API
    void loadH5( const std::string& filePath, bool append = false );

    SIMIL_API
    std::vector< uint32_t > getSubset( const std::string& name ) const;

    SIMIL_API
    std::vector< TimeFrame > getTimeFrame( const std::string& name ) const;

    SIMIL_API
    GIDMapRange subsets( void ) const;

    SIMIL_API
    TimeFrameRange timeFrames( void ) const;

  protected:

    std::map< std::string, std::vector< uint32_t >> _subsets;
    std::map< std::string, std::vector< std::pair< float, float >>> _timeFrames;

  };


}



#endif /* __SIMIL_SUBSETEVENTMANAGER__ */
