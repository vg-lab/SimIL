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


#include "H5Activity.h"

#include <assert.h>

namespace simil
{

  H5Activity::H5Activity( const H5Network& network,
              const std::string& fileName_,
              const std::string& pattern_ )
  : _fileName( fileName_ )
  , _pattern( pattern_ )
  , _network( &network )
  { }

  H5Activity::~H5Activity( void )
  { }


  std::string H5Activity::fileName( void ) const
  {
    return _fileName;
  }

  std::string H5Activity::pattern( void ) const
  {
    return _pattern;
  }


  H5Spikes::H5Spikes( const H5Network& network,
            const std::string& fileName_,
            const std::string& pattern_ )
  : H5Activity( network, fileName_, pattern_ )
  , _startTime( 0.f )
  , _endTime( 0.f )
  , _totalRecords( 0 )
  { }

  H5Spikes::~H5Spikes( void )
  { }

  void H5Spikes::Load( void )
  {
//    const std::vector< std::string >& names = _network->_groupNames;

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

    std::vector< std::string > tempNames;

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

      // Check if group name contains the pattern word.
      if( currentName.find( _pattern ) == std::string::npos )
        continue;

//      if( currentName != names[ i ])
//      {
//        std::cout << "Warning: name " << currentName << " does not match "
//                  << "the expected network group " << names[ i ] << std::endl;
//        continue;
//      }
      // Open the current group.
      H5::Group group = _file.openGroup( currentName );

      // Get children datasets number
      unsigned int innerDatasets = group.getNumObjs( );

      // If there is more than one object... Skip it.
      if( innerDatasets != 2 )
        continue;

      std::cout << "Found 2 datasets in " << currentName << std::endl;

      for( unsigned int j = 0; j < innerDatasets; j++ )
      {
        // Check child type.
        H5G_obj_t childType = group.getObjTypeByIdx( j );
        if( childType != H5G_obj_t::H5G_DATASET )
          break;

        // Get dataset name.
        std::string dataSetName = group.getObjnameByIdx( j );

        // Open dataset.
        H5::DataSet dataset = group.openDataSet( dataSetName );

        hsize_t dims[2];
        int totalDims = dataset.getSpace().getSimpleExtentDims( dims );

        if( totalDims > 1)
          break;

        if( dims[ 0 ] == 0 )
          break;

        if( j == 0)
        {
          if( dataset.getDataType( ).getClass( ) != H5T_class_t::H5T_INTEGER )
          {
            break;
          }

          _spikeIds.push_back( dataset );

        }
        else
        {
          if( dataset.getDataType( ).getClass( ) != H5T_class_t::H5T_FLOAT )
          {
            _spikeIds.pop_back( );
            break;
          }

          // Store current dataset.
          _spikeTimes.push_back( dataset );

          // Assume dataset is correct so far...

          records += dims[ 0 ];

          // Store group name.
          _groupNames.push_back( currentName );
          // Store current group.
          _groups.push_back( group );

        }
      }

    }

    _totalRecords = records;
    std::cout << "Loaded " << _totalRecords << " spikes." << std::endl;
  }

  TSpikes H5Spikes::spikes( void )
  {
    TSpikes result;

    if( _totalRecords == 0 )
      return result;

    std::multimap< float, uint32_t > sortedResult;

//    const std::vector< unsigned int >& offsets = _network->_offsets;

    auto& attribs = _network->_attributes;

    _startTime = std::numeric_limits< float >::max( );
    _endTime = std::numeric_limits< float >::min( );

    for( unsigned int i = 0; i < _groupNames.size( ); i++ )
    {
      auto currentName = _groupNames[ i ];

      auto att = attribs.find( currentName );
      if( att == attribs.end( ))
        continue;

      H5::DataSet& times = _spikeTimes[ i ];
      H5::DataSet& ids = _spikeIds[ i ];

      unsigned int currentOffset = std::get< H5Network::tna_offset >( att->second );

      std::cout << "----- offset " << currentName << " " << currentOffset << std::endl;

      hsize_t dimsTimes[ 2 ];
      hsize_t dimsIds[ 2 ];

      times.getSpace( ).getSimpleExtentDims( dimsTimes );
      ids.getSpace( ).getSimpleExtentDims( dimsIds );

      assert( dimsTimes[ 0 ] == dimsIds[ 0 ] );

      unsigned int numRecords = dimsTimes[ 0 ];

      std::vector< float > tempTimes( numRecords );
      std::vector< unsigned int > tempIds( numRecords );

      times.read( tempTimes.data( ), H5::PredType::IEEE_F32LE );
      ids.read( tempIds.data( ), H5::PredType::NATIVE_UINT );

      times.close( );
      ids.close( );

      _startTime = 0.0f;
//      if( *tempTimes.begin( ) < _startTime )
//        _startTime = *tempTimes.begin( );

      if( tempTimes.back( ) > _endTime )
        _endTime = tempTimes.back( );

      auto time = tempTimes.begin( );
      for( auto id : tempIds )
      {
        if( id + currentOffset > tempIds.size( ))
        {
          std::cout << "ID " << id << " out of bounds. " << id
                    << " + " << currentOffset
                    << " = " << id + currentOffset << std::endl;
        }

        sortedResult.insert( std::make_pair( *time, id + currentOffset ));

        time++;

      }

      std::cout << "Loaded dataset " << currentName << " with " << tempTimes.size( ) << std::endl;
    }

    result.reserve( sortedResult.size( ));
    for( auto spike : sortedResult )
      result.push_back( spike );

    return result;
  }

  float H5Spikes::startTime( void )
  {
    return 0;
  }

  float H5Spikes::endTime( void )
  {
    return _endTime;
  }

}
