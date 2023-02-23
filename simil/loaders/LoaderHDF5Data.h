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

#ifndef __SIMIL__LOADHDF5DATA_H__
#define __SIMIL__LOADHDF5DATA_H__

#include "LoaderSimData.h"
#include "auxiliar/H5Activity.h"
#include <simil/api.h>

namespace simil
{
  class SIMIL_API LoaderHDF5Data : public LoaderSimData
  {
  public:
    LoaderHDF5Data( );
    virtual ~LoaderHDF5Data( );

    virtual std::unique_ptr< SimulationData >
      loadSimulationData( const std::string& networkFile,
                          const std::string& activityFile="" ) override;

    virtual std::unique_ptr< Network >
    loadNetwork( const std::string& networkFile,
                                  const std::string& aux = "" ) override;

  protected:
    simil::H5Network* _h5Network;
  };

} // namespace simil

#endif /* __SIMIL__LOADHDF5DATA_H__ */
