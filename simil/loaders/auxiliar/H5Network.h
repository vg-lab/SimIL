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

#ifndef __SIMIL__H5NETWORK_H__
#define __SIMIL__H5NETWORK_H__

#include <string>
#include <vector>
#include <unordered_map>

#include <H5Cpp.h>

#include <simil/types.h>
#include <simil/simil_export.h>

namespace simil
{
  class H5Activity;
  class H5Spikes;

  class SIMIL_EXPORT H5Network
  {
    friend class H5Activity;
    friend class H5Spikes;

  public:

    enum TNetworkAttrib
    {
      tna_name = 0,
      tna_label,
      tna_gids,
      tna_offset,
      tna_group,
      tna_dataset
    };

    typedef std::tuple< std::string,
                        std::string,
                        GIDVec,
                        unsigned int,
                        H5::Group,
                        H5::DataSet > TNetworkAttributes;

    H5Network( void );
    H5Network( const std::string& fileName ,
               const std::string & pattern = "neuron");

    ~H5Network( void )
    {};

    void load( void );
    void load( const std::string& fileName ,
               const std::string & pattern = "neuron" );

    void clear( void );

    unsigned int subSetsNumber( void ) const;

    simil::TGIDSet getGIDs( void ) const;
    simil::TPosVect getComposedPositions( void );

    simil::SubsetMapRange getSubsets( void );

    std::map<std::string, glm::vec3> &getSubsetsColors();

    const std::vector< unsigned int >& offsets( void ) const;

    unsigned int composeID( unsigned int datasetIdx,
                            unsigned int localIdx ) const;

    std::string fileName( void ) const;
    std::string pattern( void ) const;

  protected:
    /** \brief Loads the network and subset in the "cells" folder of
     * a HDF5 file.
     *
     */
    void loadCellsFormat();

    /** \brief Loads the network and subset in the CA1 EFPL format of
     * a HDF5 file.
     *
     */
    void loadEFPL_CA1NetworkFormat();

    /** \brief Helper method to load the different groups in the CA1 format.
     * \param[in] dataPath Path of the dataset.
     * \param[in] idPath Path of the groups identifiers.
     * \param[in] label Label to assign to groups.
     */
    void loadEFPL_CA1_groups(const std::string& datapath, const std::string& idpath, const std::string& label);

    std::string _fileName; /** filename of hdf5 file. */
    std::string _pattern; /** pattern to load. */

    unsigned long _totalRecords; /** total number of records of the network file. */

    H5::H5File _file; /** hdf5 network file */
    std::vector< std::string > _groupNames; /** list of groups names. */
    std::vector< std::string > _datasetNames; /** list of dataset names. */

    simil::SubsetMap _subsets; /** maps of groups */
    std::map<std::string, glm::vec3> _subsetsColors; /** map of colors of the subsets. */

    std::vector< H5::Group > _groups; /** list of groups, if opened. */

    std::vector< H5::DataSet > _datasets; /** list of datasets, if opened. */

    std::vector< unsigned int > _offsets; /** offsets in the gids list. */

    std::unordered_map< std::string, TNetworkAttributes > _attributes;

    TPosVect _positions; /** gids positions list. */
    GIDVec _gids; /** gids list. */
  };

}

#endif /* __SIMIL__H5NETWORK_H__ */
