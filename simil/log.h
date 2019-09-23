/**
 * @file    log.h
 * @brief
 * @author  Pablo Toharia <pablo.toharia@urjc.es>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
            Do not distribute without further notice.
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
