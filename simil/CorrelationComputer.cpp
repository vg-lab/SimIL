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

  CorrelationComputer::CorrelationComputer( SpikeData* simData )
  : _simData( simData )
  , _subsetEvents( simData->subsetsEvents( ))
  { }


  void CorrelationComputer::compute( const std::string& subset,
                                     const std::string& event_,
                                     float deltaTime,
                                     float selectionThreshold )
  {
    EventVec events = _subsetEvents->getEvent( event_ );

    if( events.empty( ))
    {
      std::cout << "Warning: event " << event_ << " NOT found." << std::endl;
      return;
    }


    GIDVec gids = _subsetEvents->getSubset( subset );

    if( gids.empty( ))
    {
      std::cout << "Warning: subset " << subset << " NOT found." << std::endl;
      return;
    }

    TGIDUSet giduset( gids.begin( ), gids.end( ));

    const TSpikes& spikes = _simData->spikes( );

    std::map< uint32_t, unsigned int > eventNeuronSpikes;
    std::map< uint32_t, unsigned int > totalNeuronSpikes;

    // Calculate delta time inverse to avoid further division operations.
    double invDeltaTime = 1.0 / deltaTime;

    // Threshold for considering an event active during bin time.
    float threshold = deltaTime * 0.5f;

    float totalTime = _simData->endTime( );

    // Calculate bins number.
    unsigned int binsNumber = std::ceil( totalTime * invDeltaTime );

    // Initialize vector storing delta time spaced event's activity.
    std::vector< unsigned int > eventBins( binsNumber, 0 );

    std::vector< float > eventTime( binsNumber, 0.0f );

    for( auto event : events )
    {
      float acc = 0.0f;
      unsigned int counter = 0;

      float lowerBound;
      float upperBound;

      unsigned int binStart = std::floor( event.first * invDeltaTime );
      unsigned int binEnd = std::ceil( event.second * invDeltaTime );

      if( binEnd > binsNumber )
        binEnd = binsNumber;

      if( binStart > binEnd )
        continue;

      acc = binStart * deltaTime;

      for( auto binIt = eventTime.begin( ) + binStart;
           binIt != eventTime.begin( ) + binEnd; ++binIt )
      {

        lowerBound = std::max( acc, event.first );
        upperBound = std::min( acc + deltaTime, event.second );

        *binIt += ( upperBound - lowerBound );

        acc += deltaTime;
        counter++;
      }

      auto bin = eventBins.begin( );
      for( auto time : eventTime )
      {
        if( time >= threshold )
          *bin = 1;

        ++bin;
      }
    }

    // Calculate the number of active bins for the current event.
    unsigned int activeBins = 0;
    for( auto bin : eventBins )
      activeBins += bin;

    std::cout << subset <<  "-pattern " << event_ << " " << activeBins << std::endl;

    std::vector< std::set< uint32_t >> binSpikes( binsNumber );

    for( const auto& spike : spikes )
    {
      if( giduset.find( spike.second ) == giduset.end( ))
        continue;

      unsigned int bin = std::floor( spike.first * invDeltaTime );

      binSpikes[ bin ].insert( spike.second );
    }

    auto binIt = eventBins.begin( );
    for( auto neuronsPerBin : binSpikes )
    {

      if( *binIt )
      {
        for( auto neuron : neuronsPerBin )
        {
          auto it = eventNeuronSpikes.find( neuron );
          if( it == eventNeuronSpikes.end( ))
            eventNeuronSpikes.insert( std::make_pair( neuron, 1 ));
          else
            it->second++;
        }
      }

      ++binIt;
    }

    Correlation correlation;
    correlation.subsetName = subset;
    correlation.eventName = event_;

    // Calculate normalization factors by the inverse of active/inactive bins.
    float normHit = 1.0f / activeBins;
    float normFH = 1.0f / ( binsNumber - activeBins );

    // Initialize maximum value probes.
    float maxHitValue = 0.0f;
    float maxFHValue = 0.0f;
    float maxResValue = 0.0f;

    float avgHitValue = 0.0f;
    float avgFHValue = 0.0f;
    float avgResValue = 0.0f;

    // For each neuron...
    for( auto eventSpikesIt : eventNeuronSpikes )
    {

      // Calculate corresponding values according to current event activity.
      CorrelationValues values;

      // Hit value relates to spiking neurons during active event.
      values.hit = std::max( 0.0f, std::min( 1.0f, eventSpikesIt.second * normHit ));

      // False hit is related to spiking neurons when event is not active.
      values.falseHit =
          std::max( 0.0f, std::min( 1.0f, eventSpikesIt.second * normFH ));

      // Result responds to Hit minus False Hit.
      values.result = values.hit - values.falseHit;

      // Store maximum values.
      if( values.hit > maxHitValue )
        maxHitValue = values.hit;

      if( values.falseHit > maxFHValue )
        maxFHValue = values.falseHit;

      if( values.result > maxResValue )
        maxResValue = values.result;

      avgHitValue += values.hit;
      avgFHValue += values.falseHit;
      avgResValue += values.result;

      // If result is above the given threshold...
      if( values.result >= selectionThreshold )

        // Store neuron correlation value.
        correlation.values.insert( std::make_pair( eventSpikesIt.first, values ));

    }

    avgHitValue /= eventNeuronSpikes.size( );
    avgFHValue /= eventNeuronSpikes.size( );
    avgResValue /= eventNeuronSpikes.size( );

    std::cout << "Hit: Max value: " << maxHitValue << std::endl;
    std::cout << "False hit: Max value " << maxFHValue << std::endl;
    std::cout << "Result: Max value " << maxResValue << std::endl;

    std::cout << "Hit: Avg " << avgHitValue << std::endl;
    std::cout << "False hit: Avg " << avgFHValue << std::endl;
    std::cout << "Result: Avg " << avgResValue << std::endl;


    // Store the full subset correlation for filtered neurons.
    _correlations.insert( std::make_pair( subset + event_, correlation ));

    std::cout << "Computed correlation for event " << subset
              << " with "<< correlation.values.size( ) << " elements."
              << std::endl;
  }

  std::vector< Correlation >
  CorrelationComputer::correlate( const std::string& subsetName,
                                  const std::vector< std::string >& eventNames,
                                  float deltaTime,
                                  float selectionThreshold )
  {
    std::vector< uint32_t > gids =
        _subsetEvents->getSubset( subsetName );

    if( gids.empty( ))
    {
      std::cerr << "Error: Destination GID subset " << subsetName
                << " is empty or does not exist!" << std::endl;
      exit( -1 );
    }

    std::vector< std::string > composedNames( eventNames.size( ));
    auto it = composedNames.begin( );
    for( auto name : eventNames )
    {
      *it = subsetName + name;
      ++it;
    }

    std::vector< Correlation* > impliedCorrelations;
    it = composedNames.begin( );
    for( auto eventName : eventNames )
    {
      auto res = _correlations.find( *it );

      if( res == _correlations.end( ))
      {
        compute( subsetName, eventName, deltaTime, selectionThreshold );

        res = _correlations.find( *it );
      }

      impliedCorrelations.push_back( &res->second );

      ++it;
    }

    unsigned int counter = 0;
    std::vector< Correlation > result( eventNames.size( ));
    for( auto& c : result )
    {
      c.subsetName = subsetName;
      c.eventName = eventNames[ counter ];

      ++counter;
    }

    for( auto gid : gids )
    {
      CorrelationValues max;
      max.falseHit = max.hit = max.result = std::numeric_limits< float >::min( );

      int maxNumber = -1;

      counter = 0;
      for( auto correlation : impliedCorrelations )
      {
        auto res = correlation->values.find( gid );
        if( res != correlation->values.end( ) && res->second > max )
        {
          max = res->second;
          maxNumber = counter;
        }

        ++counter;
      }

      if( maxNumber > -1 )
        result[ maxNumber ].values.insert( std::make_pair( gid, max ));
    }

    return result;
  }


  Correlation*
  CorrelationComputer::correlation( const std::string& subsetName )
  {
    auto it = _correlations.find( subsetName );

    if( it == _correlations.end( ))
      return nullptr;

    return &it->second;
  }

  std::vector< std::string > CorrelationComputer::correlationNames( void )
  {
    std::vector< std::string > result;

    for( auto c : _correlations )
      result.push_back( c.first );

    return result;
  }

}
