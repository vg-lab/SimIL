/*
 * @file  LoadSimData.h
 * @brief
 * @author Aaron Sújar <aaron.sujar@urjc.es>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 *          Do not distribute without further notice.
 */

#ifndef __SIMIL__LOADSIMDATA_H__
#define __SIMIL__LOADSIMDATA_H__

#include "../DataSet.h"

namespace simil
{
  class LoaderSimData
  {
  public:
    LoaderSimData( )
    {
    }
    virtual ~LoaderSimData( ) = 0;

    virtual SimulationData*
      loadSimulationData( const std::string& filePath_,
                          const std::string& aux ="" ) = 0;
    virtual Network* loadNetwork( const std::string& filePath_,
                                  const std::string& aux ="" ) = 0;
  };

  inline LoaderSimData::~LoaderSimData( )
  {
    std::cout << "Pure virtual destructor is called";
  }

} // namespace simil

#endif /* __SIMIL__LOADSIMDATA_H__ */
