/*
 * @file  LoadHDF5Data.h
 * @brief
 * @author Aaron Sújar <aaron.sujar@urjc.es>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 *          Do not distribute without further notice.
 */

#ifndef __SIMIL__LOADCSVDATA_H__
#define __SIMIL__LOADCSVDATA_H__

#include "LoaderSimData.h"
#include "../CSVActivity.h"
#include "../CSVNetwork.h"

namespace simil
{
  class LoaderCSVData : public LoaderSimData
  {
  public:
    LoaderCSVData( );
    ~LoaderCSVData( );

    virtual SimulationData*
      loadSimulationData( const std::string& activityFile,
                          const std::string& aux = "" ) override;

    virtual Network* loadNetwork( const std::string& aux,
                                  const std::string& activityFile="" ) override;

  protected:
    simil::CSVNetwork* _csvNetwork;
    simil::CSVActivity* _csvActivity;
  };

} // namespace simil

#endif /* __SIMIL__LOADCSVDATA_H__ */
