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

#ifndef __SIMIL_CSVNETWORK__
#define __SIMIL_CSVNETWORK__

#include "../../types.h"
#include <simil/api.h>

namespace simil
{
  class SIMIL_API CSVNetwork
  {
  public:

    CSVNetwork( const std::string& filename,
                char separator = ',',
                bool headerLine = false );

    ~CSVNetwork( );

    void load( void );

    void clear( void );

    simil::TGIDSet getGIDs( void ) const;
    simil::TPosVect getComposedPositions( void ) const;

  protected:

    std::string _fileName;
    char _separator;
    bool _headerLine;

    simil::TGIDSet _gids;
    simil::TPosVect _positions;

  };


}



#endif /* __SIMIL_CSVNETWORK__ */
