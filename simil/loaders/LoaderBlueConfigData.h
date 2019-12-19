/*
 * @file  LoadblueConfigData.h
 * @brief
 * @author Aaron Sújar <aaron.sujar@urjc.es>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 *          Do not distribute without further notice.
 */

#ifndef __SIMIL__LOADBLUECONFIGDATA_H__
#define __SIMIL__LOADBLUECONFIGDATA_H__

#include "LoaderSimData.h"

#include <brion/brion.h>
#include <brain/brain.h>

namespace simil
{
  class LoaderBlueConfigData : public LoaderSimData
  {
  public:
    LoaderBlueConfigData( );
    ~LoaderBlueConfigData( );

    virtual SimulationData*
      loadSimulationData( const std::string& filePath_,
                          const std::string& aux = "" ) override;

    virtual Network* loadNetwork( const std::string& filePath_,
                                  const std::string& targets = "" ) override;

  protected:
    brion::BlueConfig* _blueConfig;
  };

} // namespace simil

#endif /* __SIMIL__LOADBLUECONFIGDATA_H__ */
