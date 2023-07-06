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

#ifndef __SIMIL_CSVNETWORK__
#define __SIMIL_CSVNETWORK__

#include "../../types.h"
#include <simil/api.h>

namespace simil
{
  /** \class CSVNetwork
   * \brief Loads the contents of a network description in a CSV file.
   *
   */
  class SIMIL_API CSVNetwork
  {
    public:
      /** \brief CSVNetwork class constructor.
       * \param[in] filename Name on disk of the CSV file.
       * \param[in] separator Suggested separator character, can change in load().
       *
       */
      CSVNetwork( const std::string& filename,
                  char separator = ',' );

      /** \brief CSVNetwork class destructor.
       *
       */
      ~CSVNetwork( );

      /** \brief Loads the network data from the constructor params.
       *
       */
      void load( void );

      /** \brief Dumps the contents of the class to a CSV file.
       * WARNING: overwrites.
       * \param[in] filename Name of the file in the cwd.
       *
       */
      void save(const std::string filename);

      /** \brief Clears the contents of the class.
       *
       */
      void clear( void );

      /** \brief Returns the gids set.
       *
       */
      simil::TGIDSet getGIDs( void ) const;

      /** \brief Returns the gids positions vector.
       *
       */
      simil::TPosVect getComposedPositions( void ) const;

    protected:

      std::string _fileName; /** file filename. */
      char _separator;       /** suggested separator character. */

      simil::TGIDSet _gids;       /** gids set. */
      simil::TPosVect _positions; /** gids positions vector. */
  };
}

#endif /* __SIMIL_CSVNETWORK__ */
