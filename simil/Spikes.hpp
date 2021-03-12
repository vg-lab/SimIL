/*
 * Copyright (c) 2015-2020 VG-Lab/URJC.
 *
 * Authors: Sergio E. Galindo <sergio.galindo@urjc.es>
 *
 * This file is part of SimIL <https://github.com/vg-lab/SimIL>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#ifndef __SIMIL_SPIKES_H_
#define __SIMIL_SPIKES_H_

#include "types.h"
#include <simil/api.h>

namespace simil
{
  class SIMIL_API Spikes : public TSpikes
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
    , _indexSize( 100 )
    {
      buildIndex( );
    }

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

    void rebuildIndex( unsigned int newSize )
    {
      _indexSize = newSize;

      buildIndex( );
    }

  protected:

    void buildIndex( void )
    {
      _startTime = _endTime = 0;

      _references.clear( );

      if(empty()) return;

      _endTime = back( ).first;
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
