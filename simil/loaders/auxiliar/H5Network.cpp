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

#include <cassert>
#include "H5Network.h"

const char CELLS_TAG[]="cells/positions";
const char MAPS_TAG[]="cells/type_maps";
const char CONNECTIONS_TAG[]="cells/connections";

namespace simil
{
  H5Network::H5Network( void )
  : _totalRecords( 0 )
  {

  }

  H5Network::H5Network( const std::string& fileName_,
                        const std::string & pattern_ )
  : _fileName( fileName_ )
  , _pattern( pattern_ )
  , _totalRecords( 0 )
  {

  }

  void H5Network::load( const std::string& fileName_,
                        const std::string & pattern_ )
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

    // Open file
    _file = H5::H5File( _fileName, H5F_ACC_RDONLY );

    if(H5Lexists(_file.getLocId(), CELLS_TAG, H5P_DEFAULT) > 0)
    {
      loadCellsFormat();
      return;
    }

    // Get the number of outer objects.
    const unsigned int outerObjects = _file.getNumObjs( );
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
      std::string label("Unknown");

      if( currentName.find( _pattern ) != std::string::npos )
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
      const auto innerDatasets = group.getNumObjs( );

      for( unsigned int dsNum = 0; dsNum < innerDatasets; dsNum++ )
      {
        // Get dataset name.
        H5G_obj_t childType = group.getObjTypeByIdx( dsNum );

        // Check child type.
        if( childType != H5G_obj_t::H5G_DATASET )
          continue;

        const std::string dataSetName = group.getObjnameByIdx( dsNum );
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
        _gids.reserve(_gids.size() + dims[0]);
        if(dims[1] == 3)
        {
          // no gids, only positions so gids are sequential. Positions
          // will be processed later in getComposedPositions() in this case.
          for( unsigned int gid = 0; gid < dims[ 0 ]; ++gid )
          {
            _gids.emplace_back(_offsets.back() + gid);
          }
        }
        else
        {
          auto bufferFloatType = H5::PredType::IEEE_F64LE;

          assert(dataset.getTypeClass() == H5T_FLOAT);
          const auto floatType = dataset.getFloatType();
          const auto byteSize = floatType.getSize();

          if(byteSize == 4)      bufferFloatType = H5::PredType::IEEE_F32LE;
          else if(byteSize == 8) bufferFloatType = H5::PredType::IEEE_F64LE;

          auto buffer = new char[dims[0]*dims[1]*byteSize];
          std::memset(buffer, 0, dims[0]*dims[1]*byteSize);

          dataset.read( reinterpret_cast<void*>(buffer), bufferFloatType );

          TPosVect subset;
          if(byteSize == 4)
          {
            auto bufferF = reinterpret_cast<float*>(&buffer[0]);
            for(size_t idx = 0; idx < dims[0]*dims[1]; )
            {
              _gids.emplace_back(static_cast<uint32_t>(bufferF[idx]));
              idx += dims[1]-3;
              subset.emplace_back(vmml::Vector3f{bufferF[idx+0],bufferF[idx+1],bufferF[idx+2]});
              idx+=3;
            }
          }
          else
          {
            auto bufferD = reinterpret_cast<double*>(&buffer[0]);
            for(size_t idx = 0; idx < dims[0]*dims[1];)
            {
              _gids.emplace_back(static_cast<uint32_t>(bufferD[idx]));
              idx += dims[1]-3;
              subset.emplace_back(vmml::Vector3f{static_cast<float>(bufferD[idx+0]),
                                                 static_cast<float>(bufferD[idx+1]),
                                                 static_cast<float>(bufferD[idx+2])});
              idx+=3;
            }
          }
          delete [] buffer;
          _positions.insert( _positions.end( ), subset.begin( ), subset.end( ));
        }

        _subsets.insert( std::make_pair( label, _gids));
        _groupNames.push_back( currentName );
        _datasetNames.push_back( label );
        _groups.push_back( group );
        _datasets.push_back( dataset );

        TNetworkAttributes attribs =
            std::make_tuple( currentName, label, _gids, records, group, dataset );

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
    _subsetsColors.clear();

    _attributes.clear( );

    _totalRecords = 0;
    _fileName.clear( );
  }

  TGIDSet H5Network::getGIDs( void ) const
  {
    TGIDSet set;

    for(const auto &value: _gids) set.insert(value);

    return set;
  }

  TPosVect H5Network::getComposedPositions( void )
  {
    if(_positions.size() != _totalRecords)
    {
      _positions.reserve( _totalRecords );

      unsigned int currentOffset = 0;
      for( auto dataset : _datasets )
      {
        hsize_t dims[2];
        dataset.getSpace().getSimpleExtentDims( dims );

        std::vector<float> buffer(dims[0]*dims[1], 0);
        dataset.read( buffer.data( ), H5::PredType::IEEE_F32LE );

        TPosVect subset;
        for(auto bit = buffer.cbegin(); bit != buffer.cend(); )
        {
          bit += dims[1]-3;
          subset.emplace_back(vmml::Vector3f{*bit++, *bit++, *bit++});
        }

        _positions.insert( _positions.end( ), subset.begin( ), subset.end( ));

        currentOffset += dims[ 0 ];
      }
    }

    return _positions;
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
    if(_subsets.empty() && !_gids.empty()) return 1;

    return _subsets.size();
  }

  const std::vector< unsigned int >& H5Network::offsets( void ) const
  {
    return _offsets;
  }

  simil::SubsetMapRange H5Network::getSubsets( void )
  {
    if(_subsets.empty() && !_gids.empty())
    {
      _subsets.insert( std::make_pair("Unknown", _gids));
    }

    return std::make_pair( _subsets.begin( ), _subsets.end( ));
  }

  std::map<std::string, vmml::Vector3f> &H5Network::getSubsetsColors()
  {
    return _subsetsColors;
  }

  void H5Network::loadCellsFormat()
  {
    auto dataSet = _file.openDataSet(CELLS_TAG);

    hsize_t dims[2];
    dataSet.getSpace().getSimpleExtentDims( dims );
    assert(dataSet.getSpace().getSimpleExtentNdims() == 2);

    auto bufferType = H5::PredType::IEEE_F64LE;

    assert(dataSet.getTypeClass() == H5T_FLOAT);
    auto floatType = dataSet.getFloatType();
    auto byteSize = floatType.getSize();

    if(byteSize == 4)      bufferType = H5::PredType::IEEE_F32LE;
    else if(byteSize == 8) bufferType = H5::PredType::IEEE_F64LE;

    auto buffer = new char[dims[0]*dims[1]*byteSize];
    std::memset(buffer, 0, dims[0]*dims[1]*byteSize);

    // load positions
    dataSet.read( reinterpret_cast<void*>(buffer), bufferType );

    TPosVect subset;
    if(byteSize == 4)
    {
      auto bufferF = reinterpret_cast<float*>(&buffer[0]);
      for(size_t idx = 0; idx < dims[0]*dims[1]; )
      {
        _gids.emplace_back(static_cast<uint32_t>(bufferF[idx]));
        idx += dims[1]-3;
        subset.emplace_back(vmml::Vector3f{bufferF[idx+0],bufferF[idx+1],bufferF[idx+2]});
        idx+=3;
      }
    }
    else
    {
      auto bufferD = reinterpret_cast<double*>(&buffer[0]);
      for(size_t idx = 0; idx < dims[0]*dims[1];)
      {
        _gids.emplace_back(static_cast<uint32_t>(bufferD[idx]));
        idx += dims[1]-3;
        subset.emplace_back(vmml::Vector3f{static_cast<float>(bufferD[idx+0]),
                                           static_cast<float>(bufferD[idx+1]),
                                           static_cast<float>(bufferD[idx+2])});
        idx+=3;
      }
    }
    delete [] buffer;
    buffer = nullptr;
    _positions.insert( _positions.end( ), subset.begin( ), subset.end( ));

    dataSet.close();

    // group maps.
    if(H5Lexists(_file.getLocId(), MAPS_TAG, H5P_DEFAULT) > 0)
    {
      auto group = _file.openGroup(MAPS_TAG);
      const unsigned int objNum = group.getNumObjs( );

      for( unsigned int i = 0; i < objNum; i++ )
      {
        // Get object type.
        H5G_obj_t type = group.getObjTypeByIdx( i );

        // Get object name.
        const std::string name = group.getObjnameByIdx( i );

        // Check if type is a dataset.
        if( type != H5G_obj_t::H5G_DATASET )
          continue;

        // Open dataset.
        H5::DataSet innerDs = group.openDataSet( name );
        memset(dims, 0, 2*sizeof(hsize_t));
        innerDs.getSpace().getSimpleExtentDims( dims );
        assert(innerDs.getSpace().getSimpleExtentNdims() == 1);

        assert(innerDs.getTypeClass() == H5T_INTEGER);
        auto integerType = innerDs.getIntType();
        byteSize = integerType.getSize();

        if(byteSize == 4)      bufferType = H5::PredType::NATIVE_INT;
        else if(byteSize == 8) bufferType = H5::PredType::NATIVE_LONG;

        if(dims[0] == 0) continue;

        buffer = new char[dims[0]*byteSize];
        std::memset(buffer, 0, dims[0]*byteSize);

        innerDs.read( reinterpret_cast<void*>(buffer), bufferType );

        GIDVec gids;
        if(byteSize == 4)
        {
          auto bufferI = reinterpret_cast<int*>(&buffer[0]);
          for(size_t idx = 0; idx < dims[0]; )
          {
            gids.emplace_back(static_cast<uint32_t>(bufferI[idx]));
            ++idx;
          }
        }
        else
        {
          auto bufferL = reinterpret_cast<long*>(&buffer[0]);
          for(size_t idx = 0; idx < dims[0];)
          {
            gids.emplace_back(static_cast<uint32_t>(bufferL[idx]));
            ++idx;
          }
        }
        delete [] buffer;
        const auto groupName = "group " + name;
        _subsets.insert(std::make_pair(groupName, gids));
        _subsetsColors.insert(std::make_pair(groupName, vmml::Vector3f{0,0,0}));
        innerDs.close();
      }
    }

    // groups by connections
    if(H5Lexists(_file.getLocId(), CONNECTIONS_TAG, H5P_DEFAULT) > 0)
    {
      auto group = _file.openGroup(CONNECTIONS_TAG);
      const unsigned int objNum = group.getNumObjs( );

      for( unsigned int i = 0; i < objNum; i++ )
      {
        // Get object type.
        H5G_obj_t type = group.getObjTypeByIdx( i );

        // Get object name.
        const std::string name = group.getObjnameByIdx( i );

        // Check if type is a dataset.
        if( type != H5G_obj_t::H5G_DATASET )
          continue;

        // Open dataset.
        H5::DataSet innerDs = group.openDataSet( name );
        memset(dims, 0, 2*sizeof(hsize_t));
        innerDs.getSpace().getSimpleExtentDims( dims );
        assert(innerDs.getSpace().getSimpleExtentNdims() == 2);

        assert(innerDs.getTypeClass() == H5T_FLOAT);
        floatType = innerDs.getFloatType();
        byteSize = floatType.getSize();

        if(byteSize == 4)      bufferType = H5::PredType::IEEE_F32LE;
        else if(byteSize == 8) bufferType = H5::PredType::IEEE_F64LE;

        if(dims[0] == 0 || dims[1] == 0)
          continue;

        buffer = new char[dims[0]*dims[1]*byteSize];
        std::memset(buffer, 0, dims[0]*dims[1]*byteSize);

        innerDs.read( reinterpret_cast<void*>(buffer), bufferType );

        GIDVec gids;
        if(byteSize == 4)
        {
          auto bufferI = reinterpret_cast<float*>(&buffer[0]);
          for(size_t idx = 0; idx < dims[0]*dims[1]; )
          {
            gids.emplace_back(static_cast<uint32_t>(bufferI[idx]));
            gids.emplace_back(static_cast<uint32_t>(bufferI[idx+1]));
            idx += dims[1];
          }
        }
        else
        {
          auto bufferL = reinterpret_cast<double*>(&buffer[0]);
          for(size_t idx = 0; idx < dims[0]*dims[1];)
          {
            gids.emplace_back(static_cast<uint32_t>(bufferL[idx]));
            gids.emplace_back(static_cast<uint32_t>(bufferL[idx+1]));
            idx += dims[1];
          }
        }
        delete [] buffer;
        const auto groupName = "connection " + name;
        _subsets.insert(std::make_pair(groupName, gids));
        _subsetsColors.insert(std::make_pair(groupName, vmml::Vector3f{0,0,0}));
        innerDs.close();
      }
    }
  }
}
