/*
 * Copyright (c) 2015-2020 VG-Lab/URJC.
 *
 * Authors: Aaron Sujar <aaron.sujar@urjc.es>
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

#ifndef __SIMIL__LOADSIMDATA_H__
#define __SIMIL__LOADSIMDATA_H__

#include <memory>

#include "../DataSet.h"
#include <simil/api.h>

namespace simil
{
  class SIMIL_API LoaderSimData
  {
  public:
    LoaderSimData( )
    { }

    virtual ~LoaderSimData( ) = 0;

    virtual std::unique_ptr< SimulationData >
    loadSimulationData( const std::string& filePath_ ,
                        const std::string& aux = "" ) = 0;

    virtual std::unique_ptr< Network >
    loadNetwork( const std::string& filePath_ ,
                 const std::string& aux = "" ) = 0;
  };

  inline LoaderSimData::~LoaderSimData( )
  {
    std::cout << "Pure virtual destructor is called";
  }

} // namespace simil

#endif /* __SIMIL__LOADSIMDATA_H__ */
