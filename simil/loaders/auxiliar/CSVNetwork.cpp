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

#include <sys/stat.h>
#include <locale>
#include <fstream>
#include <cassert>

#include <QFile>
#include <QStringList>

struct dotSeparator: std::numpunct<char>
{
    char do_decimal_point() const { return '.'; }
};

const std::vector<char> SEPARATORS{ ',',';','\t',' ' };

namespace simil
{
  CSVNetwork::CSVNetwork( const std::string& filename,
                          char separator )
  : _fileName( filename )
  , _separator( separator )
  { }

  CSVNetwork::~CSVNetwork( )
  { }

  void CSVNetwork::load( void )
  {
    QFile file( QString::fromStdString( _fileName));
    if( !file.open( QIODevice::ReadOnly | QFile::Text))
    {
      const std::string errorMessage = std::string("Error, could not open CSV network file: ") + _fileName;
      throw std::runtime_error(errorMessage);
    }

    clear();

    bool ok = false;
    bool includesGID = false;
    unsigned int counter = 0;
    unsigned int gid = 0;

    // Tests for correct separator
    QByteArray line = file.readLine( );
    QList< QByteArray > wordList = line.split( _separator );
    if(wordList.size() < 3 || wordList.size() > 4)
    {
      for(auto candidate: SEPARATORS)
      {
        wordList = line.split( candidate );
        if( wordList.size( ) < 3 || wordList.size( ) > 4 )
          continue;

        _separator = candidate;
        break;
      }
    }
    assert(file.seek(0));

    while( !file.atEnd( ))
    {
      QStringList stringLine;
      line = file.readLine( );
      wordList = line.split( _separator );
      wordList.removeAll("");

      if( wordList.size( ) < 3 || wordList.size( ) > 4 )
      {
      	const std::string errorText = std::string("CSV error in line: ") + std::to_string(counter) +
      			                          std::string(". Please check file format and make sure all lines match the following structure for each line:") +
																			std::string(" '[GID,]X,Y,Z' where the GID is an optional field, and  X,Y,Z are the 3D coordinates.\n") +
																			std::string("Separator used: '") + _separator + "'\n";
        throw std::runtime_error(errorText);
      }

      for( const auto &word : wordList )
        stringLine.append( word.constData( ));

      vmml::Vector3f coordinates;

      includesGID = wordList.size( ) > 3;

      gid = ( includesGID ) ? stringLine[ 0 ].toUInt( &ok ) : counter;

      if(includesGID && !ok)
      {
        std::cerr << "Warning: Unable to convert gid value: " << stringLine[0].toStdString() << std::endl;
        continue;
      }

      unsigned int i = 0;
      for( auto word : stringLine )
      {
        // skips gid number to make it sequential from 0.
        if( includesGID && i == 0 )
        {
          ++i;
          continue;
        }

        const float value = word.toFloat( &ok );

        if( !ok )
        {
          std::cerr << "Warning: Value " << word.toStdString( ) << " not converted to float." << std::endl;
          continue;
        }

        coordinates[ i - (includesGID ? 1:0) ] = value;

        i++;
      }

      _gids.insert( gid );
      _positions.push_back( coordinates );

      counter++;
    }

    file.close( );
    std::cout << "CSV Read " << counter << " gids" << std::endl;
  }

  void CSVNetwork::clear( void )
  {
    _gids.clear();
    _positions.clear();
  }

  simil::TGIDSet CSVNetwork::getGIDs( void ) const
  {
    return _gids;
  }

  void CSVNetwork::save(const std::string filename)
  {
    std::ofstream nFile;
    nFile.open(filename, std::ios_base::trunc);

    if(nFile.fail())
    {
      std::cerr << "Unable to create CSV network file: " << filename << std::endl;
      return;
    }

    nFile.imbue(std::locale(nFile.getloc(), new dotSeparator()));

    auto pit = _positions.cbegin();
    for(auto git = _gids.cbegin(); git != _gids.cend(); ++git, ++pit)
    {
      auto &pos = *pit;
      nFile << std::to_string(*git) << "," << pos[0] << "," << pos[1] << "," << pos[2] << '\n';
    }
    nFile.flush();
    nFile.close();
    std::cout << "Write CSV network file: " << filename << std::endl;
  }

  simil::TPosVect CSVNetwork::getComposedPositions( void ) const
  {
    return _positions;
  }
}
