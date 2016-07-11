/*
 * ZeqEventsManager.h
 *
 *  Created on: 8 de mar. de 2016
 *      Author: sgalindo
 */

#ifndef __SIMIL__ZEQEVENTSMANAGER_H_
#define __SIMIL__ZEQEVENTSMANAGER_H_

#include <string>

#ifdef SIMIL_USE_ZEROEQ
  #include <zeroeq/zeroeq.h>

  #include <mutex>

  #include <boost/signals2/signal.hpp>
  #include <boost/bind.hpp>

#ifdef SIMIL_USE_LEXIS
#include <lexis/lexis.h>
#endif

#ifdef SIMIL_USE_GMRVLEX
  #include <gmrvlex/gmrvlex.h>
#endif

#include <thread>

#endif

class ZeqEventsManager
{
public:

  ZeqEventsManager( const std::string& zeroeqUri_ );
  ~ZeqEventsManager( );

#ifdef SIMIL_USE_ZEROEQ

  boost::signals2::signal< void ( float ) > frameReceived;
  boost::signals2::signal< void ( unsigned int ) > playbackOpReceived;

public:

  void sendFrame( const float& start, const float& end,
                  const float& current ) const;

#ifdef SIMIL_USE_GMRVLEX

  void sendPlaybackOp( zeroeq::gmrv::PlaybackOperation operation ) const;

protected:
  void _onPlaybackOpEvent( zeroeq::gmrv::ConstPlaybackOpPtr event_ );

#endif

protected:

  void _onFrameEvent( /*lexis::render::ConstFramePtr event_*/ );
  void _setZeqSession( const std::string& );


  bool _zeroeqConnection;

  std::string _session;
  zeroeq::Subscriber* _subscriber;
  zeroeq::Publisher* _publisher;

  pthread_t _subscriberThread;

  lexis::render::Frame _lastFrame;
  lexis::render::Frame _currentFrame;

  std::thread* _thread;

#endif

};


#endif /* __SIMIL__ZEQEVENTSMANAGER_H_ */
