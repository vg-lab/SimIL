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

#ifndef __SIMIL__LOADBLUECONFIGDATA_H__
#define __SIMIL__LOADBLUECONFIGDATA_H__

#include "LoaderSimData.h"

#include <brion/brion.h>
#include <brain/brain.h>
#include <simil/api.h>

namespace simil
{
  class SIMIL_API LoaderBlueConfigData : public LoaderSimData
  {
  public:
    LoaderBlueConfigData( );
    virtual ~LoaderBlueConfigData( );

    virtual std::unique_ptr< SimulationData >
    loadSimulationData( const std::string& activityFile ,
                        const std::string& aux = "" ) override;

    virtual std::unique_ptr< Network >
    loadNetwork( const std::string& aux ,
                 const std::string& activityFile = "" ) override;

  protected:
    brion::BlueConfig* _blueConfig;
  };

} // namespace simil

#endif /* __SIMIL__LOADBLUECONFIGDATA_H__ */
