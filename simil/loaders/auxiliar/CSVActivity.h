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

#ifndef __QSIMIL_CSVACTIVITY__
#define __QSIMIL_CSVACTIVITY__

#include "../../types.h"
#include "CSVNetwork.h"
#include <simil/api.h>

namespace simil
{
  /** \class CSVActivity
   * \brief Loads the activity of a network from a CSV file.
   *
   */
  class SIMIL_API CSVActivity
  {
    public:
      /** \brief CSVActivity class constructor.
       * \param[in] network Network definition in CSVNetwork class format.
       * \param[in] filename Name of CSV file.
       * \param[in] separator Suggested separator character, can change in load().
       *
       */
      CSVActivity( const CSVNetwork& network,
                   const std::string& filename,
                   char separator = ',');

      /** \brief CSVActivity class virtual destructor.
       *
       */
      virtual ~CSVActivity( void );

      /** \brief Data loading.
       *
       */
      virtual void load( void ) = 0;

      /** \brief Save the contents of the class to a CSV file.
       * \param[in] filename Name of CSV file on disk.
       *
       */
      virtual void save(const std::string filename) = 0;

      /** \brief Clears the contents of the class.
       *
       */
      virtual void clear() = 0;

      /** \brief Returns the activity start time.
       *
       */
      float startTime() const;

      /** \brief Returns the activity end time.
       *
       */
      float endTime() const;

    protected:
      const CSVNetwork& _network; /** Network definition. */
      float _endTime;             /** activity end time as float. */

      std::string _fileName; /** file filename. */
      char _separator;       /** suggested separator character. */
  };

  /** \class CSVSpikes
   * \brief Loads the spike activation activity from a CSV file, in
   * format GID,TIME.
   *
   */
  class SIMIL_API CSVSpikes : public CSVActivity
  {
    public:
      /** \brief CSVSpikes class constructor.
       * \param[in] network Network definition reference.
       * \param[in] filename Name of CSV file on disk.
       * \param[in] separator Suggested separator character, can change on load().
       *
       */
      CSVSpikes( const CSVNetwork& network,
                 const std::string& filename,
                 char separator = ',');

      /** \brief CSVSpikes class virtual destructor.
       *
       */
      virtual ~CSVSpikes( void );

      virtual void load( void );
      virtual void save(const std::string filename);

      virtual void clear();

      /** \brief Returns the spikes activity data as a vector<time,gid>.
       *
       */
      TSpikes spikes() const;

    protected:
      std::multimap< float, uint32_t > _spikes; /** spikes as a map of time - multiple gids. */
  };

  /** \class CSVVoltages
   *  \brief Loads the network voltage values from a CSV file. TODO: @felix
   *
   *
   */
  class SIMIL_API CSVVoltages : public CSVActivity
  {
      CSVVoltages(const CSVNetwork &network, const std::string &filename, const unsigned char separator);
  };
}


#endif /* __QSIMIL_CSVACTIVITY__ */
