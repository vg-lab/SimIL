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
    void loadJSON( const std::string& filePath );

    SIMIL_API
    void loadH5( const std::string& filePath );

    SIMIL_API
    void clear( void );

    SIMIL_API
    void addSubset( const std::string& name, const GIDVec& subset );

    SIMIL_API
    std::vector< uint32_t > getSubset( const std::string& name ) const;

    SIMIL_API
    void removeSubset( const std::string& name );

    SIMIL_API
    std::vector< Event > getEvent( const std::string& name ) const;

    SIMIL_API
    SubsetMapRange subsets( void ) const;

    SIMIL_API
    EventRange events( void ) const;

    unsigned int numSubsets( void ) const;
    unsigned int numEvents( void ) const;

    SIMIL_API
    std::vector< std::string > subsetNames( void ) const;

    SIMIL_API
    std::vector< std::string > eventNames( void ) const;

  protected:

    std::map< std::string, std::vector< uint32_t >> _subsets;
    std::map< std::string, std::vector< std::pair< float, float >>> _events;

  };


}



#endif /* __SIMIL_SUBSETEVENTMANAGER__ */
