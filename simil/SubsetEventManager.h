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

#ifndef __SIMIL_SUBSETEVENTMANAGER__
#define __SIMIL_SUBSETEVENTMANAGER__

#include <map>
#include <vector>

#include <simil/api.h>

#include "types.h"

namespace simil
{
  class SIMIL_API SubsetEventManager
  {

  public:

    SubsetEventManager( );

    void loadJSON( const std::string& filePath );

    void loadH5( const std::string& filePath );

    void clear( void );

    void addSubset( const std::string& name, const GIDVec& subset, const vmml::Vector3f &color = vmml::Vector3f{0,0,0} );

    std::vector< uint32_t > getSubset( const std::string& name ) const;
    vmml::Vector3f getSubsetColor( const std::string& name) const;

    void removeSubset( const std::string& name );

    std::vector< Event > getEvent( const std::string& name ) const;

    SubsetMapRange subsets( void ) const;

    EventRange events( void ) const;

    unsigned int numSubsets( void ) const;
    unsigned int numEvents( void ) const;

    float totalTime( void ) const;

    std::vector< std::string > subsetNames( void ) const;

    std::vector< std::string > eventNames( void ) const;

    std::vector< bool > eventActivity( const std::string& name,
                                       float deltaTime,
                                       float totalTime ) const;
  protected:

    std::map< std::string, std::vector< uint32_t >> _subsets;
    std::map< std::string, std::vector< std::pair< float, float >>> _events;
    std::map< std::string, vmml::Vector3f> _colors;

    float _totalTime;
  };


}



#endif /* __SIMIL_SUBSETEVENTMANAGER__ */
