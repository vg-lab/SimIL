#include "HTTPResponse.h"

using namespace boost;

HTTPResponse::HTTPResponse( )
  : m_response_stream( &m_response_buf )
{
}

unsigned int HTTPResponse::get_status_code( ) const
{
  return m_status_code;
}

const std::string& HTTPResponse::get_status_message( ) const
{
  return m_status_message;
}

const std::map< std::string, std::string >& HTTPResponse::get_headers( )
{
  return m_headers;
}

const std::istream& HTTPResponse::get_response( ) const
{
  return m_response_stream;
}

asio::streambuf& HTTPResponse::get_response_buf( )
{
  return m_response_buf;
}

void HTTPResponse::set_status_code( unsigned int status_code )
{
  m_status_code = status_code;
}

void HTTPResponse::set_status_message( const std::string& status_message )
{
  m_status_message = status_message;
}

void HTTPResponse::add_header( const std::string& name,
                               const std::string& value )
{
  m_headers[ name ] = value;
}

