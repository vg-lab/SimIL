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

#include "SimulationPlayer.h"
#include "SubsetEventManager.h"

namespace simil
{

  class CorrelationComputer
  {
  public:

    SIMIL_API
    CorrelationComputer( SpikesPlayer* simData,
                         SubsetEventManager* subsetEvents );

    SIMIL_API
    void compute( float deltaTime, float threshold = 0.0f );

    SIMIL_API
    void compute( const std::string& subset, float deltaTime,
                  float selectionThreshold );

    Correlation* correlation( const std::string& subsetName );

  protected:

    SpikesPlayer* _simPlayer;

    SubsetEventManager* _subsetEvents;

    std::unordered_map< std::string, Correlation > _correlations;

  };



}



#endif /* __SIMIL_CORRELATIONCOMPUTER__ */
