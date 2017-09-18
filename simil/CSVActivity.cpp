/*
 * @file  CSVActivity.cpp
 * @brief
 * @author Sergio E. Galindo <sergio.galindo@urjc.es>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 *          Do not distribute without further notice.
 */


#include "CSVActivity.h"

#include <QFile>
#include <QStringList>

#include <map>

namespace simil
{

  CSVActivity::CSVActivity( const CSVNetwork& network,
                            const std::string& filename,
                            char separator,
                            bool headerLine )
  : _network( &network )
  , _fileName( filename )
  , _separator( separator )
  , _headerLine( headerLine )
  { }

  CSVActivity::~CSVActivity( void )
  { }


  CSVSpikes::CSVSpikes( const CSVNetwork& network,
                        const std::string& filename,
                        char separator,
                        bool headerLine )
  : CSVActivity( network, filename, separator, headerLine )
  , _startTime( 0.0f )
  , _endTime( 0.0f )
  { }

  CSVSpikes::~CSVSpikes( void )
  { }

  void CSVSpikes::load( void )
  {

    QFile file( QString( _fileName.data( )));
    if( !file.open( QIODevice::ReadOnly | QFile::Text))
    {
      std::cerr << "Error: Could not open CSV file " << _fileName << "." << std::endl;
      return;
    }

//    if( _headerLine )
//      file.readLine( );

    unsigned int counter = 0;

    std::vector< std::vector< int >> values;

    while( !file.atEnd( ))
    {
      QStringList stringLine;
      QByteArray line = file.readLine( );
      QList< QByteArray > wordList = line.split( _separator );
      for( auto word : wordList )
        stringLine.append( word.constData( ));

      std::vector< int > row( stringLine.size( ));

      unsigned int i = 0;
      for( auto word : stringLine )
      {
        bool ok;
        float value = word.toInt( &ok );

        if( !ok )
          std::cout << "Warning: Value " << word.toStdString( ) << " not converted to float." << std::endl;

        row[ i ] = value;

        i++;
      }

      values.push_back( row );

      counter++;
    }

    std::cout << "Read " << values.size( ) * values[ 0 ].size( ) << " values." << std::endl;

    _values = values;

    _endTime = 0.72f * _values.size( );

    file.close( );

  }

  TSpikes CSVSpikes::spikes( void )
  {
    TSpikes result;

    float deltaTime = 0.72f;

    std::multimap< float, uint32_t > sortedSpikes;

    float currentTime = 0.0f;
    for( auto row : _values )
    {
      unsigned int counter = 0;

      for( int value : row )
      {
        if( value != 0 )
          sortedSpikes.insert( std::make_pair( currentTime, counter ));

        counter++;
      }
      currentTime += deltaTime;
    }

    result.reserve( sortedSpikes.size( ));
    for( auto spike : sortedSpikes )
      result.push_back( spike );

    return result;
  }

  float CSVSpikes::starTime( void )
  {
    return _startTime;
  }

  float CSVSpikes::endTime( void )
  {
    return _endTime;
  }


}
