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


#include "CSVActivity.h"

#include <sys/stat.h>
#include <cassert>
#include <locale>
#include <fstream>
#include <map>
#include <utility> // std::swap

#include <QFile>
#include <QStringList>

struct dotSeparator: std::numpunct<char>
{
    char do_decimal_point() const { return '.'; }
};

const std::vector<char> SEPARATORS{ ',',';','\t',' ' };

namespace simil
{
  CSVActivity::CSVActivity( const CSVNetwork& network,
                            const std::string& filename,
                            char separator )
  : _network( network )
  , _endTime(0.f)
  , _fileName( filename )
  , _separator( separator )
  {}

  CSVActivity::~CSVActivity( void )
  {}

  float CSVActivity::startTime() const
  {
    return 0.f;
  }

  float CSVActivity::endTime() const
  {
    return _endTime;
  }

  CSVSpikes::CSVSpikes( const CSVNetwork& network,
                        const std::string& filename,
                        char separator)
  : CSVActivity( network, filename, separator )
  {}

  CSVSpikes::~CSVSpikes( void )
  {}

  void CSVSpikes::load( void )
  {
    QFile file( QString( _fileName.data( )));
    if( !file.open( QIODevice::ReadOnly | QFile::Text))
    {
      const std::string errorMessage = std::string("Error, could not open CSV activity file: ") + _fileName;
      throw std::runtime_error(errorMessage);
    }

    clear();

    unsigned int counter = 0;
    unsigned int gidPos = 0;
    unsigned int timePos = 1;
    bool ok = false;

    // Tests for correct separator and fields position, use second line in
    // case file has a header line.
    QByteArray line = file.readLine( );
    line = file.readLine();
    QList< QByteArray > wordList = line.split( _separator );
    if(wordList.size() != 2)
    {
      for(auto candidate: SEPARATORS )
      {
        wordList = line.split( candidate );
        if( wordList.size( ) != 2 )
          continue;

        _separator = candidate;
        break;
      }
    }

    // Check order, that's why we need the second line.
    if(wordList.size() == 2)
    {
      wordList[gidPos].toInt(&ok);
      if(!ok)
        std::swap(gidPos, timePos);
    }
    assert(file.seek(0));

    while( !file.atEnd( ))
    {
      QStringList stringLine;
      line = file.readLine( );
      wordList = line.split(_separator);
      wordList.removeAll("");

      if(wordList.size() != 2)
      {
        const std::string errorText = std::string("CSV error in line: ") + std::to_string(counter) +
                                      std::string(". Please check file format and make sure all lines match the following structure for each line:") +
                                      std::string(" GID, TIME\n") + std::string("Both fields are obligatory.\n") +
                                      std::string("Separator used: '") + _separator + "'\n";
        throw std::runtime_error(errorText);
      }

      for( auto word : wordList )
        stringLine.append( word.constData( ));

      std::vector< int > row( stringLine.size( ));

      QString gidString = stringLine[ gidPos ];
      QString timeString = stringLine[ timePos ];

      bool okGID = false;
      bool okTime = false;

      const unsigned int gidValue = gidString.toInt( &okGID );
      const float timeValue = timeString.toFloat( &okTime );

      if( timeValue > _endTime )
        _endTime = timeValue;

      if( !okGID || !okTime )
      {
        std::cout << std::boolalpha << "Warning: Line " << counter << ". Invalid conversion of '" << gidString.toStdString( )
                  << "' or '" << timeString.toStdString( ) << "' results: " << okGID << "," << okTime << std::endl;
        continue;
      }

      _spikes.insert( std::make_pair( timeValue, gidValue ));

      counter++;
    }

    file.close( );
    std::cout << "CSV Read " << _spikes.size( ) << " spikes." << std::endl;
  }

  TSpikes CSVSpikes::spikes() const
  {
    TSpikes result;

    result.reserve( _spikes.size( ));
    for( auto spike : _spikes )
      result.push_back( spike );

    return result;
  }

  void CSVSpikes::save(const std::string filename)
  {
    std::ofstream aFile;
    aFile.open(filename, std::ios_base::trunc);

    if(aFile.fail())
    {
      std::cerr << "Unable to create CSV activity file: " << filename << std::endl;
      return;
    }

    aFile.imbue(std::locale(aFile.getloc(), new dotSeparator()));

    for(auto it = _spikes.cbegin(); it != _spikes.cend(); ++it)
    {
      auto &val = *it;
      aFile << std::to_string(val.second) << "," << val.first << '\n';
    }
    aFile.flush();
    aFile.close();
    std::cout << "Write CSV activity file: " << filename << std::endl;
  }

  void CSVSpikes::clear()
  {
    _endTime = 0.f;
    _spikes.clear();
  }

}
