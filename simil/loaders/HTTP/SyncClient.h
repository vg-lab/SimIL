#ifndef __SIMIL__HTTPSYNCCLIENT_H
#define __SIMIL__HTTPSYNCCLIENT_H

#include <string>
#include <map>

#include <boost/asio.hpp>

class HTTPSyncClient
{

public:
  HTTPSyncClient( );

  unsigned int get_status_code( ) const;

  const std::string& get_status_message( ) const;

  const std::map< std::string, std::string >& get_headers( );

  std::istream& get_response( );

  void set_host( const std::string& host );

  void set_port( unsigned int port );

  void set_uri( const std::string& uri );

  void set_userpointer( void* userpointer );

  std::string get_host( ) const;

  unsigned int get_port( ) const;

  const std::string& get_uri( ) const;

  void* get_userpointer( ) const;

  int execute( );

private:
  //boost::asio::streambuf& get_response_buf( );

  void set_status_code( unsigned int status_code );

  void set_status_message( const std::string& status_message );

  void add_header( const std::string& name, const std::string& value );


private:

  // Request paramters.
  std::string _host;


  std::string _uri;

  unsigned int _port;

  bool _was_cancelled;

  unsigned int _status_code; // HTTP status code.
  std::string _status_message; // HTTP status message.

  boost::asio::streambuf _response_buf;
  std::istream _response_stream;
  // Response headers.
  std::map< std::string, std::string > _headers;


  };

#endif // HTTPSYNCCLIENT_H
