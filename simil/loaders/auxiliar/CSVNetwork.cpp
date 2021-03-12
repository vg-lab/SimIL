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

    QFile file( QString::fromStdString( _fileName));
    if( !file.open( QIODevice::ReadOnly | QFile::Text))
    {
      std::cerr << "Error: Could not open CSV file " << _fileName
                << "." << std::endl;
      return;
    }

//    if( _headerLine )
//      file.readLine( );

    bool ok = false;
    bool includesGID = false;
    unsigned int counter = 0;
    unsigned int gid = 0;

    while( !file.atEnd( ))
    {
      QStringList stringLine;
      QByteArray line = file.readLine( );
      QList< QByteArray > wordList = line.split( _separator );

      if( wordList.size( ) < 3 || wordList.size( ) > 4 )
      {
        std::cerr << std::endl
                  << "CSV error in line: " << counter
                  << ". Please check file format and make sure all lines match the following structure for each line:"
                  << " '[GID,]X,Y,Z'"
                  << " where the GID is an optional field, and  X,Y,Z are the 3D coordinates."
                  << std::endl;
        exit( -1 );
      }

      for( auto word : wordList )
        stringLine.append( word.constData( ));

      vmml::Vector3f coordinates;

      includesGID = wordList.size( ) > 3;

      gid = ( includesGID ) ? stringLine[ 0 ].toUInt( &ok ) : counter;

      unsigned int i = 0;
      for( auto word : stringLine )
      {

        if( includesGID && i == 0 )
        {
          ++i;
          continue;
        }

        float value = word.toFloat( &ok );

        if( !ok )
          std::cout << "Warning: Value " << word.toStdString( ) << " not converted to float." << std::endl;

        coordinates[ i - includesGID ] = value;

        i++;
      }

      _gids.insert( gid );
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
