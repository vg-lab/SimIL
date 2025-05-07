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
#include <limits>
#include <map>
#include <tuple>
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
  , _startTime(0.f)
  , _endTime(0.f)
  , _fileName( filename )
  , _separator( separator )
  {}

  CSVActivity::~CSVActivity( void )
  {}

  float CSVActivity::startTime() const
  {
    return _startTime;
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
      const std::string errorMessage = std::string("Error, could not open CSV activity/spikes file: ") + _fileName;
      throw std::runtime_error(errorMessage);
    }

    clear();

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

    unsigned int counter = 0;
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

      QString gidString = stringLine[ gidPos ];
      QString timeString = stringLine[ timePos ];

      bool okGID = false;
      const unsigned int gidValue = gidString.toInt( &okGID );

      bool okTime = false;
      const float timeValue = timeString.toFloat( &okTime );

      if( !okGID || !okTime )
      {
        std::cout << std::boolalpha << "Warning: Line " << counter << ". Invalid conversion of '" << gidString.toStdString( )
                  << "' or '" << timeString.toStdString( ) << "' results: " << okGID << "," << okTime << std::endl;
        continue;
      }

      _startTime = std::min(_startTime, timeValue);
      _endTime = std::max(_endTime, timeValue);

      _spikes.insert( std::make_pair( timeValue, gidValue ));

      counter++;
    }

    file.close( );
    std::cout << "CSV Read " << _spikes.size( ) << " spikes. Start time: " << _startTime << " End time: " << _endTime << std::endl;
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
      std::cerr << "Unable to create CSV activity/spikes file: " << filename << std::endl;
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
    _startTime = _endTime = 0.f;
    _spikes.clear();
  }

  CSVVoltages::CSVVoltages(const CSVNetwork &network,
                           const std::string &filename,
                           const unsigned char separator)
  : CSVActivity( network, filename, separator )
  {}

  CSVVoltages::~CSVVoltages()
  {}

  void CSVVoltages::load(void)
  {
    QFile file( QString( _fileName.data( )));
    if( !file.open( QIODevice::ReadOnly | QFile::Text))
    {
      const std::string errorMessage = std::string("Error, could not open CSV activity/voltages file: ") + _fileName;
      throw std::runtime_error(errorMessage);
    }

    clear();

    // Tests for correct separator
    QByteArray line = file.readLine( );
    QList< QByteArray > wordList = line.split( _separator );
    if(wordList.size() < 2)
    {
      for(auto candidate: SEPARATORS )
      {
        wordList = line.split( candidate );
        if( wordList.size( ) < 2 )
          continue;

        _separator = candidate;
        break;
      }
    }

    if(wordList.size() < 2)
    {
      const std::string errorMessage = std::string("Error, unable to guess separator for activity/voltages file: ") + _fileName;
      throw std::runtime_error(errorMessage);
    }

    bool okHeader = false;
    wordList[0].toFloat(&okHeader);
    if(okHeader)
    {
      // Do not have header names, first line are values.
      for(int i = 1; i < wordList.size(); ++i)
      {
        m_groups.emplace_back(std::string("Group ") + std::to_string(i));
      }
    }
    else
    {
      for(int i = 1; i < wordList.size(); ++i)
      {
        auto nameStr = wordList[i].toStdString();
        nameStr.erase(std::remove(nameStr.begin(), nameStr.end(), '\n'), nameStr.end());
        m_groups.emplace_back(nameStr);
      }
    }

    assert(file.seek(0));

    unsigned int counter = 0;
    while( !file.atEnd( ))
    {
      QStringList stringLine;
      line = file.readLine( );
      wordList = line.split(_separator);
      wordList.removeAll("");

      if(wordList.size() < 2)
      {
        const std::string errorText = std::string("CSV error in line: ") + std::to_string(counter) +
                                      std::string(". Please check file format and make sure all lines match the following structure for each line:") +
                                      std::string(" TIME,VOLTAGE0, ... ,VOLTAGEN\n") +
                                      std::string("Separator used: '") + _separator + "'\n";
        throw std::runtime_error(errorText);
      }

      for( auto word : wordList )
        stringLine.append( word.constData( ));

      QString timeString = stringLine[0];

      bool okTime = false;
      const float timeValue = timeString.toFloat( &okTime );

      if( !okTime )
      {
        std::cout << std::boolalpha << "Warning: Line " << counter << ". Invalid conversion of '" << timeString.toStdString( ) 
		  << "'" << std::endl;
        continue;
      }

      bool okValue = false;
      std::vector<float> voltages;
      voltages.reserve(stringLine.size() - 1);
      for(int i = 1; i < stringLine.size(); ++i)
      {
        const float value = stringLine[i].toFloat(&okValue);
        if(okValue) voltages.push_back(value);
        else break;
      }

      if(voltages.size() != static_cast<size_t>(stringLine.size() -1))
      {
        std::cout << "Warning: Line " << counter << ". Invalid voltage conversion, values: ";
        for(int i = 1; i < stringLine.size(); ++i) std::cout << stringLine[i].toStdString() << " ";
        std::cout << std::endl;
        continue;
      }

      _startTime = std::min(_startTime, timeValue);
      _endTime = std::max(_endTime, timeValue);

      for(auto it = voltages.cbegin(); it != voltages.cend(); ++it)
      {
        m_voltages.emplace_back(timeValue, *it, std::distance(voltages.cbegin(),it));
      }

      counter++;
    }

    file.close( );
    std::cout << "CSV Read " << m_voltages.size( ) << " voltages. " << m_groups.size() << " groups.  Start time: " << _startTime << " End time: " << _endTime << std::endl;
    for(unsigned int i = 0; i < m_groups.size(); ++i)
    {
      const auto range = groupRange(m_voltages, i);
      const auto step = groupTimeStep(m_voltages, i);
      std::cout << "Group '" << m_groups[i] << "' range: [" << range.first << ", " << range.second << "] time step: " << step << std::endl;
    }
  }

  std::pair<float, float> CSVVoltages::groupRange(const TVoltages &voltages, const unsigned int group)
  {
    float minVal = std::numeric_limits<float>::max();
    float maxVal = std::numeric_limits<float>::min();

    auto computeMinMax = [&minVal, &maxVal, group](const std::tuple<float, float, int> &tuple)
    {
      if(std::get<2>(tuple) == static_cast<int>(group))
      {
        const float value = std::get<1>(tuple);
        minVal = std::min(minVal, value);
        maxVal = std::max(maxVal, value);
      }
    };
    std::for_each(voltages.cbegin(), voltages.cend(), computeMinMax);

    return std::make_pair(minVal, maxVal);
  }

  float CSVVoltages::groupTimeStep(const TVoltages &voltages, const unsigned int group)
  {
    float timestep = std::numeric_limits<float>::max();

    TVoltages filtered;
    auto insertGroupTuple = [&filtered, group](const std::tuple<float, float, int> &t)
    {
      if(std::get<2>(t) == static_cast<int>(group))
        filtered.push_back(t);
    };
    std::for_each(voltages.cbegin(), voltages.cend(), insertGroupTuple );

    for(unsigned int i = 0; i < filtered.size() - 1; ++i)
    {
      const auto timeA = std::get<0>(filtered[i]);
      const auto timeB = std::get<0>(filtered[i+1]);
      auto timeStr = std::to_string(timeB-timeA);
      auto num_digits = std::count(timeStr.cbegin(), timeStr.cend(), '0');
      float power_of_10 = std::pow(10, num_digits);
      const float roundedValue = std::round((timeB-timeA) * power_of_10) / power_of_10;

      timestep = std::min(timestep, std::abs(roundedValue));
    }

    // set a limit
    return std::max(timestep, 0.00001f);
  }

  std::pair<float, float> CSVVoltages::timeRange(const TVoltages &voltages)
  {
    auto range = std::pair<float, float>(std::numeric_limits<float>::max(), std::numeric_limits<float>::min());

    auto filterTime = [&range](const std::tuple<float, float, int> &t)
    {
      range.first = std::min(std::get<0>(t), range.first);
      range.second = std::max(std::get<0>(t), range.second);
    };
    std::for_each(voltages.cbegin(), voltages.cend(), filterTime);

    return range;
  }

  std::pair<float, float> CSVVoltages::timeRangeOfGroup(const TVoltages &voltages, const unsigned int group)
  {
    auto range = std::pair<float, float>(std::numeric_limits<float>::max(), std::numeric_limits<float>::min());

    auto filterTime = [&range, group](const std::tuple<float, float, int> &t)
    {
      if(std::get<2>(t) == static_cast<int>(group))
      {
        range.first = std::min(std::get<0>(t), range.first);
        range.second = std::max(std::get<0>(t), range.second);
      }
    };
    std::for_each(voltages.cbegin(), voltages.cend(), filterTime);

    return range;
  }

  void CSVVoltages::save(const std::string filename)
  {
    std::ofstream aFile;
    aFile.open(filename, std::ios_base::trunc);

    if(aFile.fail())
    {
      std::cerr << "Unable to create CSV activity/voltages file: " << filename << std::endl;
      return;
    }

    aFile.imbue(std::locale(aFile.getloc(), new dotSeparator()));

    aFile << "time,";
    for(auto it = m_groups.cbegin(); it != m_groups.cend(); ++it)
    {
      aFile << *it;
      if(it != m_groups.cend() - 1) aFile << ",";
    }

    for(auto it = m_voltages.cbegin(); it != m_voltages.cend(); it += m_groups.size())
    {
      for(size_t i = 0; i < m_groups.size(); ++i)
      {
        auto &val = *(it + i);
        if(i == 0) aFile << std::get<0>(val) << ",";
        aFile << std::get<1>(val);
        if(i +1 != m_groups.size()) aFile << ",";
      }
      aFile << '\n';
    }

    aFile.flush();
    aFile.close();
    std::cout << "Write CSV activity file: " << filename << std::endl;
  }

  void CSVVoltages::clear()
  {
    _startTime = _endTime = 0.f;
    m_voltages.clear();
    m_groups.clear();
  }

  TVoltages CSVVoltages::voltages() const
  {
    return m_voltages;
  }

  std::vector<std::string> CSVVoltages::groups() const
  {
    return m_groups;
  }
}
