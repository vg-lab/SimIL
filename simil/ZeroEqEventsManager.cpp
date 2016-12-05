/*
 * ZeroEqEventsManager.cpp
 *
 *  Created on: 8 de mar. de 2016
 *      Author: sgalindo
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
