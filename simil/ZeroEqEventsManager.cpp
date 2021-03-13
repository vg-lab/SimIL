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

#include "ZeroEqEventsManager.h"

ZeroEqEventsManager::ZeroEqEventsManager( const std::string&
#ifdef SIMIL_USE_ZEROEQ
                                    zeqSession_
#endif
                                  )
{

#ifdef SIMIL_USE_ZEROEQ
  _setZeqSession( zeqSession_ );
#endif
}

ZeroEqEventsManager::~ZeroEqEventsManager( )
{
#ifdef SIMIL_USE_ZEROEQ
  if( _thread )
    delete _thread;

  if( _publisher )
    delete _publisher;

  if( _subscriber )
    delete _subscriber;
#endif
}

#ifdef SIMIL_USE_ZEROEQ

void ZeroEqEventsManager::sendFrame( const float& start, const float& end,
                  const float& current ) const
{
  unsigned int factor = 10000;

  lexis::render::Frame frame;

  frame.setCurrent( current * factor );
  frame.setStart( start * factor );
  frame.setEnd( end * factor );
  frame.setDelta( 10000 );

  _publisher->publish( frame );
}

#ifdef SIMIL_USE_GMRVLEX
void ZeroEqEventsManager::sendPlaybackOp( zeroeq::gmrv::PlaybackOperation operation ) const
{
  zeroeq::gmrv::PlaybackOp op;
  op.setOp(( uint32_t ) operation );
  _publisher->publish( op );
}

void ZeroEqEventsManager::_onPlaybackOpEvent( zeroeq::gmrv::ConstPlaybackOpPtr event_ )
{
  playbackOpReceived( event_->getOp( ) );
}
#endif

void ZeroEqEventsManager::_onFrameEvent( /*lexis::render::ConstFramePtr event_*/ )
{
  float invDelta = 1.0f / float( _currentFrame.getDelta( ) );

  float start = _currentFrame.getStart( ) * invDelta;

  float percentage;

  percentage = ( float( _currentFrame.getCurrent( ) ) * invDelta - start )
      / ( float( _currentFrame.getEnd( ) ) * invDelta - start );

  _lastFrame = _currentFrame;

  assert( percentage >= 0.0f && percentage <= 1.0f );
  frameReceived( percentage );


}

void ZeroEqEventsManager::_setZeqSession( const std::string& session_ )
{

  _session = session_.empty( ) ? zeroeq::DEFAULT_SESSION : session_;

  _zeroeqConnection = true;

  _subscriber = new zeroeq::Subscriber( _session );
  _publisher = new zeroeq::Publisher( _session );

  _currentFrame.registerDeserializedCallback(
      [&]( ){ _lastFrame = _currentFrame; _onFrameEvent( ); } );

  _subscriber->subscribe( _currentFrame );

  _subscriber->subscribe( zeroeq::gmrv::PlaybackOp::ZEROBUF_TYPE_IDENTIFIER(),
                          [&]( const void* data, const size_t size )
                          {
                            _onPlaybackOpEvent( zeroeq::gmrv::PlaybackOp::create( data, size ));
                          } );

  _thread = new std::thread( [&]() { while( true ) _subscriber->receive( 10000 );});

}

#endif
