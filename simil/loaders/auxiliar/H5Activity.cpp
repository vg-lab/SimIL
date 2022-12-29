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
#include <cassert>

const char RECORDERS_TAG[]="recorders/soma_spikes";

namespace simil
{
  H5Activity::H5Activity( H5Network& network,
              const std::string& fileName_,
              const std::string & pattern_ )
  : _fileName( fileName_ )
  , _pattern( pattern_ )
  , _network( network )
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


  H5Spikes::H5Spikes(  H5Network& network,
            const std::string& fileName_,
            const std::string & pattern_ )
  : H5Activity( network, fileName_, pattern_ )
  , _startTime( 0.f )
  , _endTime( 0.f )
  , _totalRecords( 0 )
  { }

  H5Spikes::~H5Spikes( void )
  { }

  void H5Spikes::Load( void )
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

    // check if new file format and load it on success, else continue with old format.
    if(H5Lexists(_file.getLocId(), RECORDERS_TAG, H5P_DEFAULT) > 0)
    {
      loadRecordersFormat();
      return;
    }

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
      const std::string currentName = _file.getObjnameByIdx( i );

      // Check if group name contains the pattern word.
      if( currentName.find( _pattern ) == std::string::npos )
        continue;

      // Open the current group.
      H5::Group group = _file.openGroup( currentName );

      // Get children datasets number
      unsigned int innerDatasets = group.getNumObjs( );

      // If there is more than one object... Skip it.
      if( innerDatasets != 2 )
        continue;

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
    if(!_spikes.empty())
      return _spikes;

    std::multimap< float, uint32_t > sortedResult;

    auto& attribs = _network._attributes;

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

    for( auto spike : sortedResult )
      _spikes.push_back( spike );

    return _spikes;
  }

  float H5Spikes::startTime( void )
  {
    return 0;
  }

  float H5Spikes::endTime( void )
  {
    return _endTime;
  }

  void H5Spikes::loadRecordersFormat()
  {
    const auto recorders = _file.openGroup(RECORDERS_TAG);

    _startTime = std::numeric_limits< float >::max( );
    _endTime = std::numeric_limits< float >::min( );

    std::multimap< float, uint32_t > sortedResult;

    std::map<std::string, std::string> colors;

    for(hsize_t idx = 0; idx < recorders.getNumObjs(); ++idx)
    {
      // Get object type.
      H5G_obj_t ot = recorders.getObjTypeByIdx( idx );

      // Check if type is a dataset.
      if( ot != H5G_obj_t::H5G_DATASET )
        continue;

      // Get object name.
      const std::string currentName = recorders.getObjnameByIdx( idx );

      // Open the current group.
      H5::DataSet dataSet = recorders.openDataSet( currentName );

      H5::DataType stringType = dataSet.openAttribute("label").getDataType();
      H5::DataType scalarType = dataSet.openAttribute("cell_id").getDataType();

      std::string label;
      std::string color;
      unsigned long gid;
      dataSet.openAttribute("label").read(stringType, label);
      dataSet.openAttribute("color").read(stringType, color);

      colors[label] = color;

      dataSet.openAttribute("cell_id").read(scalarType, &gid);

      auto bufferFloatType = H5::PredType::IEEE_F64LE;

      assert(dataSet.getTypeClass() == H5T_FLOAT);
      const auto floatType = dataSet.getFloatType();
      const auto byteSize = floatType.getSize();

      if(byteSize == 4)      bufferFloatType = H5::PredType::IEEE_F32LE;
      else if(byteSize == 8) bufferFloatType = H5::PredType::IEEE_F64LE;

      hsize_t dims[2];
      memset(dims, 0, 2*sizeof(hsize_t));
      dataSet.getSpace().getSimpleExtentDims( dims );
      assert(dataSet.getSpace().getSimpleExtentNdims() == 2);
      if(dims[0] == 0 || dims[1] == 0) continue;

      auto buffer = new char[dims[0]*dims[1]*byteSize];
      std::memset(buffer, 0, dims[0]*dims[1]*byteSize);

      dataSet.read( reinterpret_cast<void*>(buffer), bufferFloatType );

      TPosVect subset;
      if(byteSize == 4)
      {
        auto bufferF = reinterpret_cast<float*>(&buffer[0]);
        for(size_t bidx = 0; bidx < dims[0]*dims[1]; )
        {
          bidx++;
          sortedResult.emplace(std::make_pair(bufferF[bidx], gid));
          bidx++;
        }
      }
      else
      {
        auto bufferD = reinterpret_cast<double*>(&buffer[0]);
        for(size_t bidx = 0; bidx < dims[0]*dims[1];)
        {
          bidx++;
          sortedResult.emplace(std::make_pair(static_cast<float>(bufferD[bidx]), gid));
          bidx++;
        }
      }
      delete [] buffer;
    }

    if(!sortedResult.empty())
    {
      for(auto item: sortedResult)
      {
        _startTime = std::min(_startTime, item.first);
        _endTime = std::max(_endTime, item.first);
        _spikes.emplace_back(item);
      }
    }

    assignColors(colors);

    std::cout << "total spikes: " << _spikes.size();
  }

  void H5Activity::assignColors(
      const std::map<std::string, std::string> &groupsColor)
  {
    if(groupsColor.empty()) return;

    auto &subsetColors = _network.getSubsetsColors();

    for(const auto &groupColor: groupsColor)
    {
      const auto name = groupColor.first;
      const auto color = groupColor.second.substr(1);
      const auto cellName = name.substr(0, name.find('_'));

      auto subsetIts = _network.getSubsets( );
      for (auto it = subsetIts.first; it != subsetIts.second; ++it )
      {
        // do magic
        const auto gName = (*it).first;
        // only color groups...
        if(gName.find("group") != std::string::npos)
        {
          // ...that contains the name
          if(gName.find(cellName) != std::string::npos)
          {
            std::stringstream ss(color);
            int num;

            ss >> std::hex >> num;

            const int r = num / 0x10000;
            const int g = (num / 0x100) % 0x100;
            const int b = num % 0x100;

            subsetColors[gName] = vmml::Vector3f(r/255., g/255., b/255.);
            break;
          }
        }
      }
    }
  }

}
