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

#include "LoaderSimData.h"
#include "../H5Activity.h"

namespace simil
{
  class LoaderHDF5Data : public LoaderSimData
  {
  public:
    LoaderHDF5Data( );
    ~LoaderHDF5Data( );

    virtual SimulationData*
      loadSimulationData( const std::string& networkFile,
                          const std::string& activityFile="" ) override;

    virtual Network* loadNetwork( const std::string& networkFile,
                                  const std::string& aux = "" ) override;

  protected:
    simil::H5Network* _h5Network;
  };

} // namespace simil

#endif /* __SIMIL__LOADHDF5DATA_H__ */
