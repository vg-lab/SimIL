/*
 * @file	Spikes.hpp
 * @brief
 * @author Sergio E. Galindo <sergio.galindo@urjc.es> 
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 *					Do not distribute without further notice.
 */

#ifndef __SIMIL_SPIKES_H_
#define __SIMIL_SPIKES_H_

#include "types.h"

namespace simil
{
  class Spikes : public TSpikes
  {
  public:

    Spikes( )
    : TSpikes( )
    , _indexSize( 10000 )
    , _invTime( 0.0f )
    , _delta( 1.0f )
    , _startTime( 0.0f )
    , _endTime( 0.0f )
    { }

    Spikes( const TSpikes& other )
    : TSpikes( other )
    {
      buildIndex( );
    }

//    Spikes& operator=( const TSpikes& other )
//    {
//      clear( );
//      std::copy( other.begin( ), other.end( ), begin( ));
//
//      buildIndex( );
//
//      return *this;
//    }

    TSpikes::const_iterator elementAt( float time ) const
    {
      if( _references.empty( ))
        return end( );

      TSpikes::const_iterator result = begin( );

      float perc =
          std::max( 0.0f, std::min( 1.0f, ( time - _startTime ) * _invTime ));

      unsigned int index = std::floor( perc * _indexSize );

      TSpikes::const_iterator ref = *( _references.begin( ) + index );

      TSpikes::const_iterator it = ref;
      while( it->first < time )
      {
        result = it;
        ++it;
      }

      return result;
    }

    const std::vector< TSpikes::const_iterator >& refData( void ) const
    {
      return _references;
    }

  protected:

    void buildIndex( void )
    {
      _indexSize = 100;//size( ) * 0.001f;

      _startTime = 0;
      _endTime = back( ).first;

      _references.clear( );
      _references.resize( _indexSize );

      _invTime = 1.0f / ( _endTime - _startTime );

      _delta = ( _endTime - _startTime ) / _indexSize;

      float acc = 0.0f;
      std::vector< float > limits( _indexSize, 0.0f );

      for( auto& limit : limits )
      {
        limit = acc;
        acc += _delta;
      }

      TSpikes::iterator spikeIt = begin( );
      TSpikes::iterator last = spikeIt;
      auto limitIt = limits.begin( );
      for( auto& ref : _references )
      {
        while( spikeIt->first <= *limitIt && spikeIt != end( ))
        {

          last = spikeIt;
          ++spikeIt;

        }

        ref = last;

        ++limitIt;
      }

      std::cout << "Built index with " << _indexSize
                << " elements." << std::endl;


//      elementAt( 3000.f );
    }

    std::vector< TSpikes::const_iterator > _references;

    unsigned int _indexSize;
    float _invTime;
    float _delta;

    float _startTime;
    float _endTime;
  };
}

#endif /* __SIMIL_SPIKES_H_ */
