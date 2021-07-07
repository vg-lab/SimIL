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
#include <cassert>

template<class T> void ignore( const T& ) { }

ZeroEqEventsManager::ZeroEqEventsManager( const std::string& session )
#ifdef SIMIL_USE_ZEROEQ
: _thread{nullptr}
#endif
{
#ifdef SIMIL_USE_ZEROEQ
  _setZeqSession( session );
#else
  ignore(session);
#endif
}

#ifdef SIMIL_USE_ZEROEQ
ZeroEqEventsManager::ZeroEqEventsManager( std::shared_ptr<zeroeq::Subscriber> subscriber, std::shared_ptr<zeroeq::Publisher> publisher )
: _thread{nullptr}
{
  _setZeqSession(subscriber, publisher);
}
#endif

ZeroEqEventsManager::~ZeroEqEventsManager( )
{
#ifdef SIMIL_USE_ZEROEQ
  deinitializeZeroEQ();
#endif
}

#ifdef SIMIL_USE_ZEROEQ

void ZeroEqEventsManager::sendFrame( const float& start, const float& end,
                  const float& current ) const
{
  if(!_publisher) return;

  const unsigned int factor = 10000;

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
  if(!_publisher) return;

  zeroeq::gmrv::PlaybackOp op;
  op.setOp( static_cast<uint32_t>(operation) );
  _publisher->publish( op );
}

void ZeroEqEventsManager::_onPlaybackOpEvent( zeroeq::gmrv::ConstPlaybackOpPtr event_ )
{
  playbackOpReceived( event_->getOp( ) );
}
#endif

void ZeroEqEventsManager::_onFrameEvent( /*lexis::render::ConstFramePtr event_*/ )
{
  const float invDelta = 1.0f / float( _currentFrame.getDelta( ) );
  const float start = _currentFrame.getStart( ) * invDelta;

  const float percentage = ( static_cast<float>( _currentFrame.getCurrent( ) ) * invDelta - start )
                         / ( static_cast<float>( _currentFrame.getEnd( ) ) * invDelta - start );

  _lastFrame = _currentFrame;

  assert( percentage >= 0.0f && percentage <= 1.0f );
  frameReceived( percentage );
}

void ZeroEqEventsManager::_setZeqSession( const std::string& session )
{
  deinitializeZeroEQ();

  const auto zeqSession = session.empty() ? zeroeq::DEFAULT_SESSION : session;

  try
  {
    _subscriber = std::make_shared<zeroeq::Subscriber>( zeqSession );
    _publisher = std::make_shared<zeroeq::Publisher>( zeqSession );
  }
  catch(const std::exception &e)
  {
    std::cerr << "ZeroEqEventsManager::_setZeqSession(string) -> " << e.what() << ". "
              << __FILE__ << ":" << __LINE__ << std::endl;
    // No need to clean up as we're throwing from the constructor
    // (if *new* fails memory is freed, no leak and no object).
    throw;
  }

  subscribeToEvents();

  if(_subscriber)
  {
    _thread = new std::thread( [&](){ while( true ) { _subscriber->receive( 10000 ); } });
  }
}

void ZeroEqEventsManager::_setZeqSession( std::shared_ptr<zeroeq::Subscriber> subscriber,
                                          std::shared_ptr<zeroeq::Publisher> publisher)
{
  if(!subscriber && !publisher)
  {
    _setZeqSession(zeroeq::DEFAULT_SESSION);
  }
  else
  {
    deinitializeZeroEQ();

    _subscriber = subscriber;
    _publisher  = publisher;

    subscribeToEvents();
  }
}

void ZeroEqEventsManager::subscribeToEvents()
{
  _currentFrame.registerDeserializedCallback(
      [&]( ){ _lastFrame = _currentFrame; _onFrameEvent( ); } );

  if(_subscriber)
  {
    _subscriber->subscribe( _currentFrame );

    _subscriber->subscribe( zeroeq::gmrv::PlaybackOp::ZEROBUF_TYPE_IDENTIFIER(),
                            [&]( const void* data, const size_t size )
                            { _onPlaybackOpEvent( zeroeq::gmrv::PlaybackOp::create( data, size )); });
  }
}

void ZeroEqEventsManager::deinitializeZeroEQ()
{
  if( _thread )
  {
    _thread->~thread();
    delete _thread;
  }

  if( _publisher )
  {
    _publisher = nullptr;
  }

  if( _subscriber )
  {
    _subscriber->unsubscribe(zeroeq::gmrv::PlaybackOp::ZEROBUF_TYPE_IDENTIFIER());
    _subscriber = nullptr;
  }
}

#endif
