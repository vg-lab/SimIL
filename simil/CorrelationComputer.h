/*
 * @file  CorrelationComputer.h
 * @brief
 * @author Sergio E. Galindo <sergio.galindo@urjc.es>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 *          Do not distribute without further notice.
 */
#ifndef __SIMIL_CORRELATIONCOMPUTER__
#define __SIMIL_CORRELATIONCOMPUTER__

#include "types.h"

#include <unordered_map>

#include "SimulationData.h"
#include "SubsetEventManager.h"

namespace simil
{

  class CorrelationComputer
  {
  public:

    SIMIL_API
    CorrelationComputer( SpikeData* simData );

    SIMIL_API
    void compute( const std::string& subset,
                  const std::string& event,
                  float deltaTime = 0.125f,
                  float selectionThreshold = 0.0f );

    std::vector< std::string > correlationNames( void );

    Correlation* correlation( const std::string& subsetName );

  protected:

    SpikeData* _simData;

    SubsetEventManager* _subsetEvents;

    std::map< std::string, Correlation > _correlations;

  };



}



#endif /* __SIMIL_CORRELATIONCOMPUTER__ */
