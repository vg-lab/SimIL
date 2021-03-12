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

#include "H5Network.h"

namespace simil
{

  H5Network::H5Network( void )
  : _totalRecords( 0 )
  {

  }

  H5Network::H5Network( const std::string& fileName_,
                        const std::string& pattern_ )
  : _fileName( fileName_ )
  , _pattern( pattern_ )
  , _totalRecords( 0 )
  {

  }

  H5Network::~H5Network( void )
  {

  }

  void H5Network::load( const std::string& fileName_,
                        const std::string& pattern_ )
  {
    _fileName = fileName_;
    _pattern = pattern_;

    load( );
  }

  void H5Network::load( void )
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

    // Get the number of outer objects.
    unsigned int outerObjects = _file.getNumObjs( );

    unsigned int records = 0;

    // For each objects...
    for( unsigned int i = 0; i < outerObjects; i++ )
    {
      // Get object type.
      H5G_obj_t ot = _file.getObjTypeByIdx( i );

      // Check if type is a group.
      if( ot != H5G_obj_t::H5G_GROUP )
        continue;

      // Get object name.
      std::string currentName = _file.getObjnameByIdx( i );
      std::string label;

      // Check if group name contains the pattern word.
      if( currentName.find( _pattern ) == std::string::npos )
        continue;

      // Open the current group.
      H5::Group group = _file.openGroup( currentName );

      if( group.attrExists( "name" ))
      {
        H5::DataType type = group.openAttribute( "name" ).getDataType( );

        H5std_string newName("");
        group.openAttribute( "name" ).read( type, newName );
        label = newName;
      }
      // Get children datasets number
      unsigned int innerDatasets = group.getNumObjs( );

      // If there is more than one object... Skip it.
//      if( innerDatasets != 1 )
//        continue;

      // Check child type.
      H5G_obj_t childType = group.getObjTypeByIdx( 0 );
      if( childType != H5G_obj_t::H5G_DATASET )
        continue;

      for( unsigned int dsNum = 0; dsNum < innerDatasets; dsNum++ )
      {

      // Get dataset name.
        std::string dataSetName = group.getObjnameByIdx( dsNum );

        if( dataSetName.find( "position" ) == std::string::npos )
        {
          std::cout << "Positions dataset not found: " << dataSetName << std::endl;
          continue;
        }

        // Open dataset.
        H5::DataSet dataset = group.openDataSet( dataSetName );

        hsize_t dims[2];
        dataset.getSpace().getSimpleExtentDims( dims );



        // Assume dataset is correct so far...

        _offsets.push_back( records );

        GIDVec subsetGids( dims[ 0 ] );
        for( unsigned int gid = 0; gid < dims[ 0 ]; ++gid )
        {
           subsetGids[ gid ] = _offsets.back( ) + gid;
        }

        _subsets.insert( std::make_pair( label, std::move( subsetGids )));

        // Store group name.
        _groupNames.push_back( currentName );

        _datasetNames.push_back( label );

        // Store current group.
        _groups.push_back( group );
        // Store current dataset.
        _datasets.push_back( dataset );

        TNetworkAttributes attribs =
            std::make_tuple( currentName, label, subsetGids, records, group, dataset );

        _attributes.insert( std::make_pair( currentName, attribs ));

        records += dims[ 0 ];
      }
    }

    _totalRecords = records;

  }

  void H5Network::clear( void )
  {
    _groupNames.clear( );
    _datasetNames.clear( );
    _groups.clear( );
    _offsets.clear( );
    _datasets.clear( );
    _subsets.clear( );

    _attributes.clear( );

    _totalRecords = 0;
    _fileName.clear( );
  }

  TGIDSet H5Network::getGIDs( void ) const
  {
    TGIDSet result;

    for( unsigned int i = 0; i < _totalRecords; i++ )
    {
      result.insert( i );
    }

    return result;
  }

  TPosVect H5Network::getComposedPositions( void ) const
  {
    TPosVect result;

    if( _totalRecords == 0 )
      return result;

    result.reserve( _totalRecords );

    unsigned int currentOffset = 0;
    for( auto dataset : _datasets )
    {
      hsize_t dims[2];
      dataset.getSpace().getSimpleExtentDims( dims );

      std::vector< vmml::Vector3f > subset( dims[ 0 ] );
      dataset.read( subset.data( ), H5::PredType::IEEE_F32LE );

      result.insert( result.end( ), subset.begin( ), subset.end( ));

      currentOffset += dims[ 0 ];
    }

    return result;
  }

  std::string H5Network::fileName( void ) const
  {
    return _fileName;
  }

  std::string H5Network::pattern( void ) const
  {
    return _pattern;
  }

  unsigned int H5Network::composeID( unsigned int datasetIdx,
                                     unsigned int localIdx ) const
  {
    return _offsets[ datasetIdx ] + localIdx;
  }

  unsigned int H5Network::subSetsNumber( void ) const
  {
    return _datasets.size( );
  }

  const std::vector< unsigned int >& H5Network::offsets( void ) const
  {
    return _offsets;
  }

  simil::SubsetMapRange H5Network::getSubsets( void ) const
  {
    return std::make_pair( _subsets.begin( ), _subsets.end( ));
  }

}
