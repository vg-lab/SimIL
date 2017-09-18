/*
 * @file  CSVNetwork.cpp
 * @brief
 * @author Sergio E. Galindo <sergio.galindo@urjc.es>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 *          Do not distribute without further notice.
 */

#include "CSVNetwork.h"

#include <QFile>
#include <QStringList>


namespace simil
{

  CSVNetwork::CSVNetwork( const std::string& filename,
                          char separator,
                          bool headerLine )
  : _fileName( filename )
  , _separator( separator )
  , _headerLine( headerLine )
  { }

  CSVNetwork::~CSVNetwork( )
  { }

  void CSVNetwork::load( void )
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

    while( !file.atEnd( ))
    {
      QStringList stringLine;
      QByteArray line = file.readLine( );
      QList< QByteArray > wordList = line.split( _separator );
      for( auto word : wordList )
        stringLine.append( word.constData( ));

      vmml::Vector3f coordinates;

      unsigned int i = 0;
      for( auto word : stringLine )
      {
        bool ok;
        float value = word.toFloat( &ok );

        if( !ok )
          std::cout << "Warning: Value " << word.toStdString( ) << " not converted to float." << std::endl;

        coordinates[ i ] = value;

        i++;
      }

      _gids.insert( counter );
      _positions.push_back( coordinates );

      counter++;
    }

    file.close( );
  }

  void CSVNetwork::clear( void )
  {

  }

  simil::TGIDSet CSVNetwork::getGIDs( void ) const
  {
    return _gids;
  }

  simil::TPosVect CSVNetwork::getComposedPositions( void ) const
  {
    return _positions;
  }


}
