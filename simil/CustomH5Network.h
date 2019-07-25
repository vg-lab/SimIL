/*
 * @file	CustomH5Network.h
 * @brief
 * @author Sergio E. Galindo <sergio.galindo@urjc.es> 
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 *					Do not distribute without further notice.
 */

#ifndef CUSTOMH5NETWORK_H_
#define CUSTOMH5NETWORK_H_

#include <H5Cpp.h>

#include "types.h"

namespace simil
{
  class CustomH5Network
  {
  public:

    void load( void );
    void load( const std::string& fileName ,
               const std::string& pattern = "positions" );

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
    H5::DataSet _dataset;

    TGIDSet _gids;
    TPosVect _positions;
  };


}



#endif /* CUSTOMH5NETWORK_H_ */
