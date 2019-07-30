/*
 * @file  GDFActivity.h
 * @brief
 * @author Sergio E. Galindo <sergio.galindo@urjc.es>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 *          Do not distribute without further notice.
 */

#include "GDFActivity.h"

#include <fstream>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

namespace simil
{

  GDFActivity::GDFActivity( void )
  : _totalRecords( 0 )
  , _startTime( 0.0f )
  , _endTime( 0.0f )
  { }

  GDFActivity::~GDFActivity( void )
  {

  }

  void GDFActivity::load( const std::string& filePath,
                          const std::string& separator,
                          bool readFirstLine )
  {

    std::ifstream file( filePath, std::ios_base::in );

    std::string line;
    if( !readFirstLine )
      std::getline( file, line );

    std::vector< std::string > tokens;

    _startTime = 0.0f;
    _endTime = 0.0f;

    while( std::getline( file, line, '\n' ))
    {
//      std::cout << "'" << line << "'" << std::endl;

      boost::split( tokens, line , boost::is_any_of( separator ));

//      std::cout << "tokens: " << tokens.size( ) << std::endl;
//      for( auto token : tokens )
//        std::cout << " '" << token << "'" << std::endl;

      boost::trim( tokens[ 0 ]);
      boost::trim( tokens[ 1 ]);

      std::string stringTime = tokens[ 0 ];
      std::string stringGID = tokens[ 1 ];

      float time = boost::lexical_cast< float >( tokens[ 1 ]);
      unsigned int gid = boost::lexical_cast< unsigned int >( tokens[ 0 ]);

      if( time > _endTime )
        _endTime = time;

      _spikes.emplace_back( std::make_pair( time, gid ));
    }

    file.close( );

    std::cout << "Loaded " << _spikes.size( ) << " spikes." << std::endl;

  }

  const std::vector< std::string >& GDFActivity::fileName( void ) const
  {
    return _fileNames;
  }

  const std::vector< std::string >& GDFActivity::pattern( void ) const
  {
    return _separators;
  }

  TSpikes GDFActivity::spikes( void )
  {
    std::sort( _spikes.begin( ), _spikes.end( ),
      [ ]( const Spike& a, const Spike& b){ return a.first < b.first; });

    return _spikes;
  }

  float GDFActivity::startTime( void ) const
  {
    return _startTime;
  }

  float GDFActivity::endTime( void ) const
  {
    return _endTime;
  }

}
