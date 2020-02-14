/*
 * Copyright (c) 2015-2020 GMRV/URJC.
 *
 * Authors: Pablo Toharia <pablo.toharia@urjc.es>
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

#ifndef __SIMIL_ERROR__
#define __SIMIL_ERROR__

#include <stdexcept>
#include <iostream>

#ifdef DEBUG
  #define SIMIL_LOG( msg )                                       \
    std::cerr << "SIMIL "                                        \
              << __FILE__ << "("                                 \
              << __LINE__ << "): "                               \
              << msg << std::endl;
#else
  #define SIMIL_LOG( msg )
#endif


#define SIMIL_THROW( msg )                                       \
  {                                                              \
    SIMIL_LOG( msg );                                            \
    throw std::runtime_error( msg );                             \
  }


#define SIMIL_CHECK_THROW( cond, errorMsg )                      \
    {                                                            \
      if ( ! (cond) ) SIMIL_THROW( errorMsg );                   \
    }


#ifdef DEBUG
  #define SIMIL_DEBUG_CHECK( cond, errorMsg )                    \
{                                                                \
  SIMIL_CHECK_THROW( cond, errorMsg )                            \
}
#else
  #define SIMIL_DEBUG_CHECK( cond, errorMsg )
#endif


#endif // __SIMIL_ERROR__
