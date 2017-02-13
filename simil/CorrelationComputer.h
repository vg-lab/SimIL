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

  struct Correlation
  {
  public:

    std::string subsetName;

    std::unordered_map< uint32_t, float > correlation;

  };

  class CorrelationComputer
  {
  public:

    SIMIL_API
    CorrelationComputer( SpikesPlayer* simData,
                         SubsetEventManager* subsetEvents );

    SIMIL_API
    void compute( void );

    SIMIL_API
    void compute( const std::string& subset );

  protected:

    SpikesPlayer* _simPlayer;

    SubsetEventManager* _subsetEvents;

    std::unordered_map< std::string, Correlation > _correlations;

  };



}



#endif /* __SIMIL_CORRELATIONCOMPUTER__ */
