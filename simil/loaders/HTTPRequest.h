#ifndef __SIMIL__HTTPREQUEST_H__
#define __SIMIL__HTTPREQUEST_H__

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

#include <thread>
#include <mutex>
#include <memory>
#include <iostream>

#include "HTTPResponse.h"
#include "HTTPClient.h"

using namespace boost;

class HTTPRequest;
class HTTPResponse;

typedef void(*Callback) (const HTTPRequest& request,
    const HTTPResponse& response,
    const system::error_code& ec);


class HTTPRequest {
	friend class HTTPClient;

	static const unsigned int DEFAULT_PORT = 80;

    HTTPRequest(asio::io_service& ios, unsigned int id);
	
public:
    void set_host(const std::string& host) ;

    void set_port(unsigned int port);

    void set_handlerId( int id);
    void set_userpointer( void * userpointer);

    void set_uri(const std::string& uri) ;

    void set_callback(Callback callback);

    std::string get_host() const ;

    unsigned int get_port() const ;

    const std::string& get_uri() const ;

    unsigned int get_id() const ;

    int get_handlerId() const ;

    void* get_userpointer() const;

    void execute() ;

    void cancel() ;
	
private:
	void on_host_name_resolved(
		const boost::system::error_code& ec,
		asio::ip::tcp::resolver::iterator iterator);
	
	void on_connection_established(
		const boost::system::error_code& ec,
        asio::ip::tcp::resolver::iterator );
	
	void on_request_sent(const boost::system::error_code& ec,
        std::size_t );
	
	void on_status_line_received(
		const boost::system::error_code& ec,
        std::size_t );
	
	void on_headers_received(const boost::system::error_code& ec,
        std::size_t );
	
	void on_response_body_received(
		const boost::system::error_code& ec,
        std::size_t );

    void on_finish(const boost::system::error_code& ec);

private:
	// Request paramters. 
	std::string m_host;
	unsigned int m_port;
	std::string m_uri;

	// Object unique identifier. 
	unsigned int m_id;

	// Callback to be called when request completes. 
	Callback m_callback;

    //Handle ID
    int m_handleid;
    //Pointer user defined
    void * m_userpointer;

	// Buffer containing the request line.
	std::string m_request_buf;

	asio::ip::tcp::socket m_sock;
	asio::ip::tcp::resolver m_resolver;

	HTTPResponse m_response;

	bool m_was_cancelled;
	std::mutex m_cancel_mux;

	asio::io_service& m_ios;
};


#endif /* __SIMIL__HTTPCLIENT_H__ */
