/*
 * @file  CorrelationComputer.cpp
 * @brief
 * @author Sergio E. Galindo <sergio.galindo@urjc.es>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 *          Do not distribute without further notice.
 */

#include "CorrelationComputer.h"

namespace simil
{

  CorrelationComputer::CorrelationComputer( SpikesPlayer* simData,
                                            SubsetEventManager* subsetEvents )
  : _simPlayer( simData )
  , _subsetEvents( subsetEvents )
  { }

  void CorrelationComputer::compute( void )
  {
    for( const auto& event : _subsetEvents->eventNames( ))
    {
      compute( event );
    }
  }

  void CorrelationComputer::compute( const std::string& subset )
  {
    EventVec timeFrames = _subsetEvents->getEvent( subset );

    if( timeFrames.empty( ))
    {
      std::cout << "Warning: event " << subset << " NOT found." << std::endl;
      return;
    }

    uint32_t neuronsNum = _simPlayer->gids( ).size( );

//    std::vector< unsigned int > totalNeuronSpikes( neuronsNum, 0 );
    std::vector< float > invTotalNeuronSpikes( neuronsNum, 0.0f );

    std::vector< unsigned int > eventNeuronSpikes( neuronsNum, 0 );

//    for( const auto& spike : _simPlayer->spikes( ))
//    {
//      totalNeuronSpikes[ spike.second ]++;
//    }



    const TSpikes& spikes = _simPlayer->spikes( );

    auto begin = spikes.begin( );
    auto end = spikes.begin( );
    end++;

    for( auto& tf : timeFrames )
    {

      while( begin->first < tf.first && begin != spikes.end( ) )
        begin++;

      while( end->first < tf.second && end != spikes.end( ))
        end++;
//      auto spikesInRange = _simPlayer->spikesBetween( tf.first, tf.second );

      if( begin == end )
        break;

      for( auto spike = begin; spike != end; ++spike )
      {
        eventNeuronSpikes[ spike->second ]++;
      }
    }

    Correlation correlation;
    correlation.subsetName = subset;

//    std::vector< float >::const_iterator invTSpikesIt =
//        invTotalNeuronSpikes.begin( );

    std::vector< unsigned int >::const_iterator eventSpikesIt =
        eventNeuronSpikes.begin( );

    float normalization = 1.0f / timeFrames.size( );

    std::cout << "Calculating..." << std::endl;
//    std::cout << "Calculated:";
    float maxValue = 0.0f;
    for( unsigned int i = 0; i < neuronsNum; i++, eventSpikesIt++ )
    {
      float value = *eventSpikesIt * normalization;
      if( value > maxValue )
        maxValue = value;

      if( value > 0.f )
      {
        correlation.correlation.insert( std::make_pair( i, value ));
//        std::cout << " " << i << ":" << value;
      }
    }
//    std::cout << std::endl;

    std::cout << "Max value: " << maxValue << std::endl;

    _correlations.insert( std::make_pair( subset, correlation ));
    std::cout << "SUCCESS: Computed correlation for event " << subset
              << " with "<< correlation.correlation.size( )
              << " elements."
              << std::endl;
  }

}

