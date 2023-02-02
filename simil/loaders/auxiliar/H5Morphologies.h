//
// Created by grial on 25/11/22.
//

#ifndef NEUROTESSMESH_H5MORPHOLOGIES_H
#define NEUROTESSMESH_H5MORPHOLOGIES_H

#include <string>
#include <utility>
#include <vector>
#include <map>
#include <ostream>
#include <H5Cpp.h>

enum class MorphologyType
{
  SOMA ,
  DENDRITES ,
  AXON ,
  UNDEFINED
};

enum class NeuronType
{
  GRANULE_CELL ,
  GOLGI_CELL ,
  PURKINJE_CELL ,
  STELLATE_CELL ,
  BASKET_CELL
};

static const NeuronType ALL_NEURON_TYPES[] = {
  NeuronType::GRANULE_CELL ,
  NeuronType::GOLGI_CELL ,
  NeuronType::PURKINJE_CELL ,
  NeuronType::STELLATE_CELL ,
  NeuronType::BASKET_CELL
};

struct MorphologyNeurite
{
  MorphologyType type;
  int64_t parent;
  std::vector< double > radii;
  std::vector< double > x;
  std::vector< double > y;
  std::vector< double > z;

  MorphologyNeurite( ) = default;

  MorphologyNeurite(
    MorphologyType type ,
    int64_t parent ,
    std::vector< double > radii ,
    std::vector< double > x ,
    std::vector< double > y ,
    std::vector< double > z );
};

struct MorphologyNeuron
{
  std::vector< MorphologyNeurite > neurites;
};

struct Neuron
{
  NeuronType type;
  uint32_t id;
  double x;
  double y;
  double z;
};

class H5Morphologies
{
  static constexpr const char* MORPHOLOGIES_GROUP = "morphologies";
  static constexpr const char* BRANCHES_GROUP = "branches";
  static constexpr const char* BRANCH_LABELS_ATTRIBUTE = "branch_labels";
  static constexpr const char* PARENT_ATTRIBUTE = "parent";

  static constexpr const char* CELLS_GROUP = "cells";
  static constexpr const char* PLACEMENT_GROUP = "placement";
  static constexpr const char* CELL_TYPES_ATTRIBUTE = "types";
  static constexpr const char* POSITIONS_DATASET = "positions";
  static constexpr const char* IDENTIFIERS_DATASET = "identifiers";

  std::string _fileName;
  std::string _pattern;

  H5::H5File _file;

  bool _loaded;

  std::map< NeuronType , MorphologyNeuron > _morphologies;
  std::vector< Neuron > _neurons;

  static MorphologyType getMorphologyType( std::vector< const char* >& array );

  void loadMorphologies( );

  void loadNeurons( );

public:

  static std::string fetchPascalCaseName( NeuronType type );

  static NeuronType fetchTypeBySnakeCaseName( const std::string& name );


  H5Morphologies( std::string file , std::string pattern );

  void load( );

  const std::map< NeuronType , MorphologyNeuron >&
  getMorphologies( ) const;

  const std::vector< Neuron >& getNeurons( ) const;

};


#endif //NEUROTESSMESH_H5MORPHOLOGIES_H
