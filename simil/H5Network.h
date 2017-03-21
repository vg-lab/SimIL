/*
 * @file	H5Network.h
 * @brief
 * @author Sergio E. Galindo <sergio.galindo@urjc.es> 
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 *					Do not distribute without further notice.
 */

#ifndef __SIMIL__H5NETWORK_H__
#define __SIMIL__H5NETWORK_H__

#include <string>
#include <vector>
#include <unordered_map>

#include <H5Cpp.h>

#include "types.h"

namespace simil
{

  class H5Activity;
  class H5Spikes;

  class H5Network
  {
    friend class H5Activity;
    friend class H5Spikes;

  public:

    H5Network( void );
    H5Network( const std::string& fileName ,
               const std::string& pattern = "neuron" );

    ~H5Network( void );

    void load( void );
    void load( const std::string& fileName ,
               const std::string& pattern = "neuron" );

    void clear( void );

    unsigned int subSetsNumber( void ) const;

    simil::TGIDSet getGIDs( void ) const;
    simil::TPosVect getComposedPositions( void ) const;

    simil::SubsetMapRange getSubsets( void ) const;

    const std::vector< unsigned int >& offsets( void ) const;

    unsigned int composeID( unsigned int datasetIdx,
                            unsigned int localIdx ) const;

    std::string fileName( void ) const;
    std::string pattern( void ) const;

  protected:

    std::string _fileName;
    std::string _pattern;

    unsigned int _totalRecords;

    H5::H5File _file;
    std::vector< std::string > _groupNames;
    simil::SubsetMap _subsets;

    std::vector< H5::Group > _groups;
    std::vector< H5::DataSet > _datasets;

    std::vector< unsigned int > _offsets;


  };

}

#endif /* __SIMIL__H5NETWORK_H__ */
