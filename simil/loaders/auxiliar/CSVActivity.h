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

#ifndef __QSIMIL_CSVACTIVITY__
#define __QSIMIL_CSVACTIVITY__

#include "../../types.h"
#include "CSVNetwork.h"

namespace simil
{

  class CSVActivity
  {

  public:

    CSVActivity( const CSVNetwork& network,
                 const std::string& filename,
                 char separator = ',',
                 bool headerLine = false );

    virtual ~CSVActivity( void );

    virtual void load( void ) = 0;

  protected:

    const CSVNetwork* _network;

    std::string _fileName;
    char _separator;
    bool _headerLine;

  };

  class CSVSpikes : public CSVActivity
  {
  public:

    CSVSpikes( const CSVNetwork& network,
               const std::string& filename,
               char separator = ',',
               bool headerLine = true );

    ~CSVSpikes( void );

    void load( void );

    TSpikes spikes( void );
    float startTime( void );
    float endTime( void );

  protected:

    float _startTime;
    float _endTime;

//    std::vector< std::vector< int >> _values;
    std::multimap< float, uint32_t > _spikes;
  };

}


#endif /* __QSIMIL_CSVACTIVITY__ */
