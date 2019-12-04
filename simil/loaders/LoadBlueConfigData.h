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

#include "LoadSimData.h"

#include <brion/brion.h>
#include <brain/brain.h>

namespace simil
{
  class LoadBlueConfigData : public LoadSimData
  {
  public:
    LoadBlueConfigData( );
    ~LoadBlueConfigData( );

    virtual SimulationData*
      LoadSimulationData( const std::string& filePath_,
                          const std::string& target = "" ) override;

    /*virtual DataSet* LoadNetwork( const std::string& filePath_,
                                  const std::string& target = "" ) override;*/

  protected:
    brion::BlueConfig* _blueConfig;
  };

} // namespace simil

#endif /* __SIMIL__LOADBLUECONFIGDATA_H__ */
