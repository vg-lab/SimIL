/*
 * @file  CSVNetwork.h
 * @brief
 * @author Sergio E. Galindo <sergio.galindo@urjc.es>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 *          Do not distribute without further notice.
 */

#ifndef __SIMIL_CSVNETWORK__
#define __SIMIL_CSVNETWORK__

#include "../simil/types.h"

namespace simil
{
  class CSVNetwork
  {
  public:

    CSVNetwork( const std::string& filename,
                char separator = ',',
                bool headerLine = false );

    ~CSVNetwork( );

    void load( void );

    void clear( void );

    simil::TGIDSet getGIDs( void ) const;
    simil::TPosVect getComposedPositions( void ) const;

  protected:

    std::string _fileName;
    char _separator;
    bool _headerLine;

    simil::TGIDSet _gids;
    simil::TPosVect _positions;

  };


}



#endif /* __SIMIL_CSVNETWORK__ */
