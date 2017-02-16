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

  void CorrelationComputer::compute( float deltaTime, float threshold )
  {
    for( const auto& event : _subsetEvents->eventNames( ))
    {
      compute( event, deltaTime, threshold );
    }
  }

  void CorrelationComputer::compute( const std::string& subset,
                                     float deltaTime,
                                     float selectionThreshold )
  {
    EventVec events = _subsetEvents->getEvent( subset );

    if( events.empty( ))
    {
      std::cout << "Warning: event " << subset << " NOT found." << std::endl;
      return;
    }

    const TSpikes& spikes = _simPlayer->spikes( );

    uint32_t neuronsNum = _simPlayer->gids( ).size( );

    std::vector< unsigned int > eventNeuronSpikes( neuronsNum, 0 );

    // Calculate delta time inverse to avoid further division operations.
    float invDeltaTime = 1.0f / deltaTime;

    // Threshold for considering an event active during bin time.
    float threshold = deltaTime * 0.5f;

    float totalTime = _simPlayer->endTime( );

    // Calculate bins number.
    unsigned int binsNumber = std::ceil( totalTime * invDeltaTime );

    // Initialize vector storing delta time spaced event's activity.
    std::vector< unsigned int > eventBins( binsNumber, 0 );

    auto binIt = eventBins.begin( );
    auto binStartIt = eventBins.begin( );
    auto binEndIt = eventBins.begin( );

    unsigned int counter = 0;

    // For each event...
    for( auto& event : events )
    {
      // Stop if shorter activity data.
      if( event.first > totalTime )
        break;

      // Calculate corresponding bin for event bounds.
      unsigned int binStart = std::floor( event.first * invDeltaTime );
      unsigned int binEnd = std::floor( event.second * invDeltaTime );

      // Avoid out of range issues.
      if( binEnd >= eventBins.size( ))
        binEnd = eventBins.size( ) - 1;

      // Initialize bin start and end bin iterators
      binStartIt = eventBins.begin( ) + binStart;
      binEndIt = eventBins.begin( ) + binEnd;

      unsigned int currentBin = binStart;

      // First bin
      if( event.first - ( binStart * deltaTime ) < threshold )
        *binStartIt = 1;

      currentBin++;

      // Intermediate and last bins
      for( binIt = binStartIt ; binIt != binEndIt; ++binIt )
      {
        if( event.second - ( currentBin * deltaTime ) > threshold )
          *binIt = 1;

        currentBin++;
      }

      counter++;
    }

    // Calculate the number of active bins for the current event.
    unsigned int activeBins = 0;
    for( auto bin : eventBins )
      activeBins += bin;

    auto begin = spikes.begin( );
    auto end = spikes.begin( );

    float currentStart = 0;
    float currentEnd = deltaTime;

    // For each bin...
    for( auto& bin : eventBins )
    {
      // Check if the event is active along bin length.
      if( !bin )
        continue;

      // Go to the bin immediately previous to event start time.
      while( begin->first < currentStart && begin != spikes.end( ) )
        begin++;

      end = begin;

      // Go to the bin immediately previous to event end time.
      while( end->first < currentEnd && end != spikes.end( ))
        end++;

      unsigned int totalSpikingNeurons = 0;

      // Set to avoid neurons spiking more than once per bin.
      std::set< uint32_t > spikingNeurons;
      for( auto spike = begin; spike != end; ++spike )
      {
        // If neuron is not already in the set...
        if( spikingNeurons.find( spike->second ) == spikingNeurons.end( ))
        {
          // Add it to the set.
          spikingNeurons.insert( spike->second );

          // Record neuron activity.
          eventNeuronSpikes[ spike->second ]++;
        }
        totalSpikingNeurons++;
      }

      // Proceed to next bin.
      currentStart = currentEnd;
      currentEnd += deltaTime;
    }

    Correlation correlation;
    correlation.subsetName = subset;

    std::vector< unsigned int >::const_iterator eventSpikesIt =
        eventNeuronSpikes.begin( );

    // Calculate normalization factors by the inverse of active/inactive bins.
    float normHit = 1.0f / activeBins;
    float normFH = 1.0f / ( binsNumber - activeBins );

    // Initialize maximum value probes.
    float maxHitValue = 0.0f;
    float maxFHValue = 0.0f;
    float maxResValue = 0.0f;


    // For each neuron...
    for( unsigned int i = 0; i < neuronsNum; i++, ++eventSpikesIt )
    {

      // Calculate corresponding values according to current event activity.
      CorrelationValues values;

      // Hit value relates to spiking neurons during active event.
      values.hit = std::max( 0.0f, std::min( 1.0f, *eventSpikesIt * normHit ));

      // False hit is related to spiking neurons when event is not active.
      values.falseHit =
          std::max( 0.0f, std::min( 1.0f, *eventSpikesIt * normFH ));

      // Result responds to Hit minus False Hit.
      values.result = values.hit - values.falseHit;

      // Store maximum values.
      if( values.hit > maxHitValue )
        maxHitValue = values.hit;

      if( values.falseHit > maxFHValue )
        maxFHValue = values.falseHit;

      if( values.result > maxResValue )
        maxResValue = values.result;

      // If result is above the given threshold...
      if( values.result >= selectionThreshold )

        // Store neuron correlation value.
        correlation.values.insert( std::make_pair( i, values ));

    }

    std::cout << "Hit: Max value: " << maxHitValue << std::endl;
    std::cout << "False hit: Max value " << maxFHValue << std::endl;
    std::cout << "Result: Max value " << maxResValue << std::endl;

    // Store the full subset correlation for filtered neurons.
    _correlations.insert( std::make_pair( subset, correlation ));

    std::cout << "Computed correlation for event " << subset
              << " with "<< correlation.values.size( ) << " elements."
              << std::endl;
  }


  Correlation*
  CorrelationComputer::correlation( const std::string& subsetName )
  {
    auto it = _correlations.find( subsetName );

    if( it == _correlations.end( ))
      return nullptr;

    return &it->second;
  }

}
