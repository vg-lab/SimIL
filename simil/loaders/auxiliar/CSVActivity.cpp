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

    _spikes.clear( );

    unsigned int counter = 0;

    float endTime = 0.0f;

    while( !file.atEnd( ))
    {
      QStringList stringLine;
      QByteArray line = file.readLine( );
      QList< QByteArray > wordList = line.split( _separator );
      for( auto word : wordList )
        stringLine.append( word.constData( ));

      std::vector< int > row( stringLine.size( ));

      QString gidString = stringLine[ 0 ];
      QString timeString = stringLine[ 1 ];

      bool okGID;
      bool okTime;

      unsigned int gidValue = gidString.toInt( &okGID );
      float timeValue = timeString.toFloat( &okTime );

      if( timeValue > endTime )
        endTime = timeValue;

      if( !okGID || !okTime )
      {
        std::cout << "Warning: Value " << gidString.toStdString( )
                  << " or " << timeString.toStdString( )
                  << " not converted to float." << std::endl;
        continue;
      }


      _spikes.insert( std::make_pair( timeValue, gidValue ));

      counter++;
    }

    _startTime = 0;
    _endTime = endTime;

    std::cout << "Read " << _spikes.size( ) << " values." << std::endl;

    file.close( );

  }

  TSpikes CSVSpikes::spikes( void )
  {
    TSpikes result;

    result.reserve( _spikes.size( ));
    for( auto spike : _spikes )
      result.push_back( spike );

    return result;
  }

  float CSVSpikes::startTime( void )
  {
    return _startTime;
  }

  float CSVSpikes::endTime( void )
  {
    return _endTime;
  }


}
