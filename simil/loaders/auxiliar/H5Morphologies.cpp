//
// Created by grial on 25/11/22.
//

#include "H5Morphologies.h"

#include <iostream>
#include <utility>
#include <vector>

#include <cstring>

MorphologyNeurite::MorphologyNeurite(
  MorphologyType type_ ,
  int64_t parent_ ,
  std::vector< double > radii_ ,
  std::vector< double > x_ ,
  std::vector< double > y_ ,
  std::vector< double > z_ )
  : type( type_ )
  , parent( parent_ )
  , radii( std::move( radii_ ))
  , x( std::move( x_ ))
  , y( std::move( y_ ))
  , z( std::move( z_ ))
{ }

std::string H5Morphologies::fetchPascalCaseName( NeuronType type )
{
  switch ( type )
  {
    case NeuronType::GRANULE_CELL:
      return "GranuleCell";
    case NeuronType::GOLGI_CELL:
      return "GolgiCell";
    case NeuronType::PURKINJE_CELL:
      return "PurnkinjeCell";
    case NeuronType::STELLATE_CELL:
      return "StellateCell";
    case NeuronType::BASKET_CELL:
      return "BasketCell";
  }
  return "";
}

NeuronType H5Morphologies::fetchTypeBySnakeCaseName( const std::string& name )
{
  if ( name == "granule_cell" ) return NeuronType::GRANULE_CELL;
  if ( name == "golgi_cell" ) return NeuronType::GOLGI_CELL;
  if ( name == "purnkinje_cell" ) return NeuronType::PURKINJE_CELL;
  if ( name == "stellate_cell" ) return NeuronType::STELLATE_CELL;
  if ( name == "basket_cell" ) return NeuronType::BASKET_CELL;
  return NeuronType::GOLGI_CELL;
}

MorphologyType
H5Morphologies::getMorphologyType( std::vector< const char* >& array )
{
  const char* SOMA_TYPE = "soma";
  const char* AXON_TYPE = "axon";
  const char* DENDRITE_TYPE = "dendrites";
  for ( const auto& item: array )
  {
    if ( strcmp( item , DENDRITE_TYPE ) == 0 )
      return MorphologyType::DENDRITES;
    if ( strcmp( item , AXON_TYPE ) == 0 ) return MorphologyType::AXON;
    if ( strcmp( item , SOMA_TYPE ) == 0 ) return MorphologyType::SOMA;
  }
  return MorphologyType::UNDEFINED;
}

void H5Morphologies::loadMorphologies( )
{
  _morphologies.clear( );
  auto root = _file.openGroup( MORPHOLOGIES_GROUP );

  for ( const auto& cellType: ALL_NEURON_TYPES )
  {
    auto cellGroupName = fetchPascalCaseName( cellType );

    if ( !H5Lexists( root.getLocId( ) , cellGroupName.c_str( ) ,
                     H5P_DEFAULT ))
      continue;
    //if ( !root.nameExists( cellGroupName )) continue;

    auto cellGroup = root.openGroup( cellGroupName );

    if ( !H5Lexists( cellGroup.getLocId( ) , BRANCHES_GROUP , H5P_DEFAULT ))
      //if ( !cellGroup.nameExists( BRANCHES_GROUP ))
    {
      std::cerr << "Warning: couldn't find branches group in "
                << cellGroupName
                << "." << std::endl;
      cellGroup.close( );
      continue;
    }

    MorphologyNeuron neuron;
    auto branchesGroup = cellGroup.openGroup( BRANCHES_GROUP );
    neuron.neurites.resize( branchesGroup.getNumObjs( ));
    for ( uint32_t cellIndex = 0; cellIndex < branchesGroup.getNumObjs( );
          ++cellIndex )
    {
      auto cellId = branchesGroup.getObjnameByIdx( cellIndex );
      auto cell = branchesGroup.openGroup( cellId );

      if ( !cell.attrExists( BRANCH_LABELS_ATTRIBUTE ))
      {
        std::cout << "Warning: couldn't find labels for neurite " << cellId
                  << " of neuron " << cellGroupName
                  << "." << std::endl;
        continue;
      }

      // Read branch labels
      auto labels = cell.openAttribute( BRANCH_LABELS_ATTRIBUTE );
      hsize_t dimension = 0;
      labels.getSpace( ).getSimpleExtentDims( &dimension );
      std::vector< const char* > labelsData;
      labelsData.resize( dimension );
      labels.read( labels.getStrType( ) , labelsData.data( ));
      auto type = getMorphologyType( labelsData );
      labels.close( );

      // Read parent id
      auto parentAttribute = cell.openAttribute( PARENT_ATTRIBUTE );
      int64_t parent;
      parentAttribute.read( parentAttribute.getIntType( ) , &parent );
      parentAttribute.close( );

      // Read labelsData
      auto radii = cell.openDataSet( "radii" );
      auto x = cell.openDataSet( "x" );
      auto y = cell.openDataSet( "y" );
      auto z = cell.openDataSet( "z" );

      auto radiiAmount = radii.getStorageSize( ) / sizeof( double );
      auto xAmount = x.getStorageSize( ) / sizeof( double );
      auto yAmount = y.getStorageSize( ) / sizeof( double );
      auto zAmount = z.getStorageSize( ) / sizeof( double );

      std::vector< double > radiiData;
      std::vector< double > xData;
      std::vector< double > yData;
      std::vector< double > zData;
      radiiData.resize( radiiAmount , 0.0 );
      xData.resize( xAmount , 0.0 );
      yData.resize( yAmount , 0.0 );
      zData.resize( zAmount , 0.0 );

      radii.read( radiiData.data( ) , radii.getFloatType( ));
      x.read( xData.data( ) , x.getFloatType( ));
      y.read( yData.data( ) , y.getFloatType( ));
      z.read( zData.data( ) , z.getFloatType( ));
      cell.close( );

      uint32_t cellIdAsInt = std::stoul( cellId );

      neuron.neurites[ cellIdAsInt ] = MorphologyNeurite(
        type ,
        parent ,
        std::move( radiiData ) ,
        std::move( xData ) ,
        std::move( yData ) ,
        std::move( zData )
      );
    }

    branchesGroup.close( );
    cellGroup.close( );

    _morphologies.emplace( cellType , std::move( neuron ));
  }
  root.close( );
}

void H5Morphologies::loadNeurons( )
{
  _neurons.clear( );

  auto root = _file.openGroup( CELLS_GROUP );

  std::map< uint32_t , NeuronType > typesById;
  std::map< std::string , NeuronType > typesByName;

  // Read cell names
  auto types = root.openAttribute( CELL_TYPES_ATTRIBUTE );
  hsize_t dimension = 0;
  types.getSpace( ).getSimpleExtentDims( &dimension );
  std::vector< const char* > labelsData;
  labelsData.resize( dimension );
  types.read( types.getStrType( ) , labelsData.data( ));

  for ( uint32_t i = 0; i < labelsData.size( ); ++i )
  {
    auto name = std::string( labelsData[ i ] );
    typesById[ i ] = fetchTypeBySnakeCaseName( name );
    typesByName[ name ] = typesById[ i ];
  }

  types.close( );

  if ( !H5Lexists( root.getLocId( ) , POSITIONS_DATASET , H5P_DEFAULT ))
  {
    std::cout << "Warning: neuron positions missing!" << std::endl;
    return;
  }

  auto positions = root.openDataSet( POSITIONS_DATASET );

  std::vector< double > positionsRaw;

  auto dataSpace = positions.getSpace( );
  hsize_t dims[2];
  dataSpace.getSimpleExtentDims( dims );
  dataSpace.close();
  positionsRaw.resize( dims[ 0 ] * dims[ 1 ] );
  positions.read( positionsRaw.data( ) , positions.getFloatType( ));

  uint32_t index = 0;

  for ( uint32_t id = positionsRaw[ 0 ]; id < dims[0]; ++id )
  {
    const uint32_t idNum = static_cast<uint32_t>(positionsRaw[index]);
    const uint32_t typeNum = static_cast<uint32_t>(positionsRaw[index + 1]);
    const auto x = positionsRaw[ index+2 ];
    const auto y = positionsRaw[ index+3 ];
    const auto z = positionsRaw[ index+4 ];
    _neurons.push_back( Neuron{ typesById[typeNum] , idNum , x , y , z } );

    index += dims[1];
  }

  positions.close( );
  root.close();

  // Read neurons
//  auto placementGroup = root.openGroup( PLACEMENT_GROUP );
//
//  for ( const auto& item: typesByName )
//  {
//    std::string name = item.first;
//    auto type = item.second;
//
//    auto typeGroup = placementGroup.openGroup( name );
//
//    if ( !H5Lexists( root.getLocId( ) , POSITIONS_DATASET , H5P_DEFAULT ))
//    {
//      std::cout << "Warning: neuron type " << name
//                << " has no position dataset." << std::endl;
//      continue;
//    }
//
//    auto positions = typeGroup.openDataSet( POSITIONS_DATASET );
//    auto identifiers = typeGroup.openDataSet( IDENTIFIERS_DATASET );
//
//    uint32_t identifiersRaw[2];
//    std::vector< double > positionsRaw;
//
//    auto dataSpace = positions.getSpace( );
//    hsize_t dims[2];
//    dataSpace.getSimpleExtentDims( dims );
//    positionsRaw.resize( dims[ 0 ] * dims[ 1 ] );
//
//    identifiers.read( identifiersRaw , identifiers.getIntType( ));
//    positions.read( positionsRaw.data( ) , positions.getFloatType( ));
//
//    uint32_t index = 0;
//    uint32_t to = identifiersRaw[ 0 ] + identifiersRaw[ 1 ];
//
//    for ( uint32_t id = identifiersRaw[ 0 ]; id < to; ++id )
//    {
//      double x = positionsRaw[ index++ ];
//      double y = positionsRaw[ index++ ];
//      double z = positionsRaw[ index++ ];
//      _neurons.push_back( Neuron{ type , id , x , y , z } );
//    }
//
//    positions.close( );
//    identifiers.close( );
//    typeGroup.close( );
//  }
//
//  placementGroup.close( );
//  root.close( );

}

H5Morphologies::H5Morphologies( std::string
                                file ,
                                std::string
                                pattern )
  : _fileName( std::move( file ))
  , _pattern( std::move( pattern ))
  , _file( )
  , _loaded( false )
  , _morphologies( )
  , _neurons( )
{
}

void H5Morphologies::load( )
{

  if ( _loaded ) return;
  if ( _fileName.empty( ))
  {
    std::cerr << "Error: file path cannot be empty." << std::endl;
    return;
  }

  if ( _pattern.empty( ))
  {
    std::cout << "Warning: an empty pattern will load all available datasets."
              << std::endl;
  }

  if ( !H5::H5File::isHdf5( _fileName ))
  {
    std::cerr << "File " << _fileName << " is not a Hdf5 file..."
              << std::endl;
    return;
  }

  _file = H5::H5File( _fileName , H5F_ACC_RDONLY);

  if ( !H5Lexists( _file.getLocId( ) , MORPHOLOGIES_GROUP , H5P_DEFAULT ))
//  if ( !_file.nameExists( MORPHOLOGIES_GROUP ))
  {
    std::cerr << "Error: couldn't find morphologies root" << std::endl;
    return;
  }

  loadMorphologies( );
  loadNeurons( );
}

const std::map< NeuronType , MorphologyNeuron >&
H5Morphologies::getMorphologies( ) const
{
  return _morphologies;
}

const std::vector< Neuron >& H5Morphologies::getNeurons( ) const
{
  return _neurons;
}
