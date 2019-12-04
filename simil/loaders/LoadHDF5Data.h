/*
 * @file  LoadHDF5Data.h
 * @brief
 * @author Aaron Sújar <aaron.sujar@urjc.es>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 *          Do not distribute without further notice.
 */

#ifndef __SIMIL__LOADHDF5DATA_H__
#define __SIMIL__LOADHDF5DATA_H__

#include "LoadSimData.h"
#include "../H5Activity.h"

namespace simil
{
  class LoadHDF5Data : public LoadSimData
  {
  public:
    LoadHDF5Data( );
    ~LoadHDF5Data( );

    virtual SimulationData*
      LoadSimulationData( const std::string& filePath_,
                          const std::string& target = "" ) override;

    /*virtual DataSet* LoadNetwork( const std::string& filePath_,
                                  const std::string& target = "" ) override;*/

  protected:
    simil::H5Network* _h5Network;
  };

} // namespace simil

#endif /* __SIMIL__LOADHDF5DATA_H__ */
