/*
 * Copyright (c) 2015-2020 GMRV/URJC.
 *
 * Authors: Sergio E. Galindo <sergio.galindo@urjc.es>
 *
 * This file is part of SimIL <https://github.com/gmrvvis/SimIL>
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

    SubsetEventManager( );

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

    float totalTime( void ) const;

    SIMIL_API
    std::vector< std::string > subsetNames( void ) const;

    SIMIL_API
    std::vector< std::string > eventNames( void ) const;

    SIMIL_API
    std::vector< bool > eventActivity( const std::string& name,
                                       float deltaTime,
                                       float totalTime ) const;

  protected:

    std::map< std::string, std::vector< uint32_t >> _subsets;
    std::map< std::string, std::vector< std::pair< float, float >>> _events;

    float _totalTime;
  };


}



#endif /* __SIMIL_SUBSETEVENTMANAGER__ */
