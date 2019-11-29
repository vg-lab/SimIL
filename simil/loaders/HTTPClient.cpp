

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
#include "HTTPClient.h"

#include <thread>
#include <mutex>
#include <memory>
#include <iostream>

using namespace boost;

HTTPClient::HTTPClient(){
		m_work.reset(new boost::asio::io_service::work(m_ios));

		m_thread.reset(new std::thread([this](){
			m_ios.run();
		}));
	}

std::shared_ptr<HTTPRequest>
        HTTPClient::create_request(unsigned int id)
	{
			return std::shared_ptr<HTTPRequest>(
				new HTTPRequest(m_ios, id));
		}

    void HTTPClient::close() {
		// Destroy the work object. 
		m_work.reset(NULL);

		// Waiting for the I/O thread to exit.
		m_thread->join();
	}

