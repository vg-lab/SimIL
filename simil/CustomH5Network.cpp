/*
 * @file	CustomH5Network.cpp
 * @brief
 * @author Sergio E. Galindo <sergio.galindo@urjc.es> 
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 *					Do not distribute without further notice.
 */

#include "CustomH5Network.h"

namespace simil
{

  void CustomH5Network::load( const std::string& fileName ,
                              const std::string& pattern )
  {
    _fileName = fileName;
    _pattern = pattern;

    load( );
  }

  void CustomH5Network::load( void )
  {
    if( _fileName.empty( ))
     {
       std::cerr << "Error: file path cannot be empty." << std::endl;
       return;
     }

     if( _pattern.empty( ))
     {
       std::cout << "Warning: an empty pattern will load all available datasets." << std::endl;
     }

     // Check whether file referenced by the path is an Hdf5 format file or not.
     if( !H5::H5File::isHdf5( _fileName ))
     {
       std::cerr << "File " << _fileName << " is not a Hdf5 file..." << std::endl;
       return;
     }

     // Create file
     _file = H5::H5File( _fileName, H5F_ACC_RDONLY );

     _dataset = _file.openDataSet( _pattern );

     hsize_t dims[2];
     _dataset.getSpace( ).getSimpleExtentDims( dims );

     std::vector< float > subset( dims[ 0 ] * dims[ 1 ] );
     _dataset.read( subset.data( ), H5::PredType::IEEE_F32LE );

     std::cout << "Loaded " << subset.size( ) << " records." << std::endl;

     _positions.reserve( subset.size( ));
     _neuronTypes.reserve( subset.size( ));

     for( unsigned int i = 0; i < subset.size( ); i += 5 )
     {
       unsigned int gid = ( unsigned int )subset[ i ] + 1;

       _gids.insert( gid );

       vmml::Vector3f position( subset[ i + 2 ], subset[ i + 3 ], subset[ i + 4 ]);

       _positions.push_back( position );

       unsigned int type = ( unsigned int ) subset[ i + 1 ] - 1;
       _neuronTypes.push_back( type );

     }
  }



  void CustomH5Network::clear( void )
  {

  }

  const simil::TGIDSet& CustomH5Network::getGIDs( void ) const
  {
//    TGIDSet result;
//
//    return result;
    return _gids;
  }

  const simil::TPosVect& CustomH5Network::getPositions( void ) const
  {
//    TPosVect result;
//
//    return result;

    return _positions;
  }

  const std::vector< long unsigned int >& CustomH5Network::getTypes( void ) const
  {
    return _neuronTypes;
  }

}


