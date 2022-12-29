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

#ifndef __SIMIL__H5ACTIVITY_H__
#define __SIMIL__H5ACTIVITY_H__

#include "../../types.h"
#include "H5Network.h"
#include <simil/api.h>

namespace simil
{
  class SIMIL_API H5Activity
  {

  public:

    H5Activity( H5Network& network,
                const std::string& fileName_,
                const std::string& pattern_ );

    virtual ~H5Activity( void );

    virtual void Load( void ) = 0;

    std::string fileName( void ) const;
    std::string pattern( void ) const;

  protected:

    void assignColors(const std::map<std::string, std::string> &groupColors);

    std::string _fileName;
    std::string _pattern;

    // 2022-12-28 @felix changed to non-const to allow activity to change subset colors.
    H5Network& _network;

    H5::H5File _file;
    std::vector< std::string > _groupNames;
    std::vector< H5::Group > _groups;
  };

  class H5Spikes : public H5Activity
  {
  public:

    H5Spikes( H5Network& network,
              const std::string& fileName_,
              const std::string& pattern_ = "neuron" );

    ~H5Spikes( void );

    void Load( void );

    TSpikes spikes( void );
    float startTime( void );
    float endTime( void );

  protected:
    /** \brief Loads HDF5 file format in recorders/soma_spikes
     * subgroup directly.
     *
     */
    void loadRecordersFormat();

    float _startTime;
    float _endTime;

    unsigned int _totalRecords;

    TSpikes _spikes;

    std::vector< H5::DataSet > _spikeTimes;
    std::vector< H5::DataSet > _spikeIds;
  };
}

#endif /* __SIMIL__H5ACTIVITY_H__ */
