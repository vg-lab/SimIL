#ifndef __SIMIL__HTTPCLIENT_H__
#define __SIMIL__HTTPCLIENT_H__

#include <boost/predef.h> // Tools to identify the OS.

// We need this to enable cancelling of I/O operations on
// Windows XP, Windows Server 2003 and earlier.
// Refer to "http://www.boost.org/doc/libs/1_58_0/
// doc/html/boost_asio/reference/basic_stream_socket/
// cancel/overload1.html" for details.
#ifdef BOOST_OS_WINDOWS
#define _WIN32_WINNT 0x0501

#if _WIN32_WINNT <= 0x0502 // Windows Server 2003 or earlier.
#define BOOST_ASIO_DISABLE_IOCP
#define BOOST_ASIO_ENABLE_CANCELIO	
#endif
#endif

#include <boost/asio.hpp>

#include "HTTPRequest.h"

#include <thread>
#include <mutex>
#include <memory>
#include <iostream>

using namespace boost;

class HTTPRequest;

class HTTPClient {
public:
    HTTPClient();

	std::shared_ptr<HTTPRequest>
        create_request(unsigned int id);

    void close();

private:
	asio::io_service m_ios;
	std::unique_ptr<boost::asio::io_service::work> m_work;
	std::unique_ptr<std::thread> m_thread;
};


#endif /* __SIMIL__HTTPCLIENT_H__ */
