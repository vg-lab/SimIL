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
const std::string CA1_NEURONS_TAG = "nodes/hippocampus_neurons/0/";
const std::string CA1_LIBRARY_TAG = "nodes/hippocampus_neurons/0/@library";

const std::vector<glm::vec3> groupsColors = {
    glm::vec3{  1,   0,   0},
    glm::vec3{  0,   1,   0},
    glm::vec3{  0,   0,   1},
    glm::vec3{  1,   0,   1},
    glm::vec3{  1,   1,   0},
    glm::vec3{  1,   0, 0.5},
    glm::vec3{  1, 0.5,   0},
    glm::vec3{  1, 0.5, 0.5},
    glm::vec3{  1, 0.5,   1},
    glm::vec3{  1,   1, 0.5},
    glm::vec3{0.5, 0.5,   0},
    glm::vec3{0.5,   0, 0.5},
    glm::vec3{  0, 0.5,   0},
    glm::vec3{  0,   0, 0.5},
    glm::vec3{  0,   0.5, 0.5}
};

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
    else
    {
        if (H5Lexists(_file.getLocId(), CA1_NEURONS_TAG.c_str(), H5P_DEFAULT) > 0) {

            loadEFPL_CA1NetworkFormat();
            return;
        }
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
              subset.emplace_back(glm::vec3{bufferF[idx+0],bufferF[idx+1],bufferF[idx+2]});
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
              subset.emplace_back(glm::vec3{static_cast<float>(bufferD[idx+0]),
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
          subset.emplace_back(glm::vec3{*bit++, *bit++, *bit++});
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

  std::map<std::string, glm::vec3> &H5Network::getSubsetsColors()
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
        subset.emplace_back(glm::vec3{bufferF[idx+0],bufferF[idx+1],bufferF[idx+2]});
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
        subset.emplace_back(glm::vec3{static_cast<float>(bufferD[idx+0]),
                                           static_cast<float>(bufferD[idx+1]),
                                           static_cast<float>(bufferD[idx+2])});
        idx+=3;
      }
    }
    delete [] buffer;
    buffer = nullptr;
    _positions.insert( _positions.end( ), subset.begin( ), subset.end( ));
    _totalRecords = _gids.size();

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

        if (byteSize == 4) {
            bufferType = H5::PredType::NATIVE_INT;
        } else if (byteSize == 8) {
            bufferType = H5::PredType::NATIVE_LONG;
        }

        if (dims[0] == 0) {
            continue;
        }

        buffer = new char[dims[0] * byteSize];
        std::memset(buffer, 0, dims[0] * byteSize);

        innerDs.read(reinterpret_cast<void*>(buffer), bufferType);

        GIDVec gids;
        if (byteSize == 4) {
          auto bufferI = reinterpret_cast<int*>(&buffer[0]);
            for (size_t idx = 0; idx < dims[0];) {
            gids.emplace_back(static_cast<uint32_t>(bufferI[idx]));
            ++idx;
          }
        } else {
          auto bufferL = reinterpret_cast<long*>(&buffer[0]);
            for (size_t idx = 0; idx < dims[0];) {
            gids.emplace_back(static_cast<uint32_t>(bufferL[idx]));
            ++idx;
          }
        }
        delete[] buffer;
        const auto groupName = "group " + name;
        _subsets.insert(std::make_pair(groupName, gids));
        _subsetsColors.insert(std::make_pair(groupName, glm::vec3{0, 0, 0}));
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
        _subsetsColors.insert(std::make_pair(groupName, glm::vec3{0,0,0}));
        innerDs.close();
      }
    }
  }

  void H5Network::loadEFPL_CA1NetworkFormat()
  {
      const std::string DS_X = CA1_NEURONS_TAG + "/x";
      const std::string DS_Y = CA1_NEURONS_TAG + "/y";
      const std::string DS_Z = CA1_NEURONS_TAG + "/z";

      auto dataSetX = _file.openDataSet(DS_X);
      auto dataSetY = _file.openDataSet(DS_Y);
      auto dataSetZ = _file.openDataSet(DS_Z);

      hsize_t dims[2];
      dataSetX.getSpace().getSimpleExtentDims(dims);
      assert(dataSetX.getSpace().getSimpleExtentNdims() == 1);

      assert(dataSetX.getTypeClass() == H5T_FLOAT);
      auto floatType = dataSetX.getFloatType();
      auto byteSize = floatType.getSize();

      auto bufferX = new double[dims[0]];
      auto bufferY = new double[dims[0]];
      auto bufferZ = new double[dims[0]];
      auto bufferM = new uint32_t[dims[0]];
      std::memset(bufferX, 0, dims[0] * byteSize);
      std::memset(bufferY, 0, dims[0] * byteSize);
      std::memset(bufferZ, 0, dims[0] * byteSize);
      std::memset(bufferM, 0, dims[0] * sizeof(uint32_t));

      dataSetX.read(reinterpret_cast<void*>(bufferX), H5::PredType::IEEE_F64LE);
      dataSetY.read(reinterpret_cast<void*>(bufferY), H5::PredType::IEEE_F64LE);
      dataSetZ.read(reinterpret_cast<void*>(bufferZ), H5::PredType::IEEE_F64LE);

      for (size_t idx = 0; idx < dims[0]; ++idx) {
          _gids.emplace_back(idx);
          _positions.emplace_back(glm::vec3{static_cast<float>(bufferX[idx]), static_cast<float>(bufferY[idx]),
                                                 static_cast<float>(bufferZ[idx])});
      }

      dataSetX.close();
      dataSetY.close();
      dataSetZ.close(); 

      loadEFPL_CA1_groups(CA1_NEURONS_TAG + "/morph_class",   CA1_LIBRARY_TAG + "/morph_class",   "Morphology Class");
      loadEFPL_CA1_groups(CA1_NEURONS_TAG + "/etype",         CA1_LIBRARY_TAG + "/etype",         "Electrical Type");
      loadEFPL_CA1_groups(CA1_NEURONS_TAG + "/layer",         CA1_LIBRARY_TAG + "/layer",         "Layer");
      loadEFPL_CA1_groups(CA1_NEURONS_TAG + "/mtype",         CA1_LIBRARY_TAG + "/mtype",         "Morphological Type");
      loadEFPL_CA1_groups(CA1_NEURONS_TAG + "/region",        CA1_LIBRARY_TAG + "/region",        "Region");
      loadEFPL_CA1_groups(CA1_NEURONS_TAG + "/synapse_class", CA1_LIBRARY_TAG + "/synapse_class", "Synapse Class");

      delete[] bufferX;
      bufferX = nullptr;
      delete[] bufferY;
      bufferY = nullptr;
      delete[] bufferZ;
      bufferZ = nullptr;
      delete[] bufferM;
      bufferM = nullptr;
  }

  void H5Network::loadEFPL_CA1_groups(const std::string &datapath, const std::string &idpath, const std::string& label)
  {
      auto dataSet = _file.openDataSet(datapath);
      auto ids = _file.openDataSet(idpath);

      hsize_t dims[2];
      ids.getSpace().getSimpleExtentDims(dims);
      assert(ids.getSpace().getSimpleExtentNdims() == 1);
      const auto groupsNum = dims[0];
      std::vector<GIDVec> groups(groupsNum);
      auto stringType = ids.getStrType();

      std::vector<char*> cStringArray(dims[0], nullptr);
      H5::DataSpace dataspace = ids.getSpace();
      ids.read(cStringArray.data(), stringType, dataspace);

      dataSet.getSpace().getSimpleExtentDims(dims);
      assert(dataSet.getSpace().getSimpleExtentNdims() == 1);
      auto bufferdata = new uint32_t[dims[0]];
      std::memset(bufferdata, 0, dims[0] * sizeof(uint32_t));
      dataSet.read(reinterpret_cast<void*>(bufferdata), H5::PredType::INTEL_U32);

      for (unsigned int idx = 0; idx < dims[0]; ++idx)
      {
          groups[bufferdata[idx]].emplace_back(idx);
      }
      delete[] bufferdata;

      for (unsigned int i = 0; i < groupsNum; ++i)
      {
          const auto groupLabel = label + " " + cStringArray[i];
          _subsets.insert(std::make_pair(groupLabel, groups[i]));
          _subsetsColors.insert(std::make_pair(groupLabel, groupsColors[i % groupsColors.size()]));
      }

      dataSet.close();
      ids.close();
  }
} // namespace simil
