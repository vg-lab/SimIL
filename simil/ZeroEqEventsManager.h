/*
 * Copyright (c) 2015-2020 GMRV/URJC.
 *
 * Authors: Sergio E. Galindo <sergio.galindo@urjc.es>
 *
 * This file is part of SimIL <https://github.com/gmrvvis/SimIL>
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

#ifndef __SIMIL__ZEROEQEVENTSMANAGER_H_
#define __SIMIL__ZEROEQEVENTSMANAGER_H_

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

#include <simil/api.h>

class SIMIL_API ZeroEqEventsManager
{
public:

  ZeroEqEventsManager( const std::string& zeroeqUri_ );
  ~ZeroEqEventsManager( );

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


#endif /* __SIMIL__ZEROEQEVENTSMANAGER_H_ */
