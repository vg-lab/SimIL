/*
 * Copyright (c) 2015-2020 GMRV/URJC.
 *
 * Authors: Sergio E. Galindo <sergio.galindo@urjc.es>
 *
 * This file is part of SimIL <https://github.com/gmrvvis/SimIL>
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
