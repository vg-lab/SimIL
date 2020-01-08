#include "SyncClient.h"


#include <iostream>
#include <istream>
#include <ostream>
#include <string>




HTTPSyncClient::HTTPSyncClient()
    : _host("localhost")
    , _uri("")
    , _port( 80 )
    , _was_cancelled( false )
    , _status_code(0)
    , _status_message("")
    , _response_stream( &_response_buf )
  {
  }

unsigned int HTTPSyncClient::get_status_code( ) const
{
  return _status_code;
}

const std::string& HTTPSyncClient::get_status_message( ) const
{
  return _status_message;
}

const std::map< std::string, std::string >& HTTPSyncClient::get_headers( )
{
  return _headers;
}

 std::istream& HTTPSyncClient::get_response( )
{
  return _response_stream;
}

 void HTTPSyncClient::set_status_code( unsigned int status_code )
 {
   _status_code = status_code;
 }

 void HTTPSyncClient::set_status_message( const std::string& status_message )
 {
   _status_message = status_message;
 }

 void HTTPSyncClient::add_header( const std::string& name, const std::string& value )
 {
   _headers[ name ] = value;
 }

 void HTTPSyncClient::set_host( const std::string& host )
 {
   _host = host;
 }

 void HTTPSyncClient::set_port( unsigned int port )
 {
   _port = port;
 }

 void HTTPSyncClient::set_uri( const std::string& uri )
 {
   _uri = uri;
 }

 std::string HTTPSyncClient::get_host( ) const
 {
   return _host;
 }

 unsigned int HTTPSyncClient::get_port( ) const
 {
   return _port;
 }

 const std::string& HTTPSyncClient::get_uri( ) const
 {
   return _uri;
 }


 int HTTPSyncClient::execute( )
 {
     try
       {

         boost::asio::io_service io_service;

         // Get a list of endpoints corresponding to the server name.
         boost::asio::ip::tcp::resolver resolver(io_service);
         boost::asio::ip::tcp::resolver::query query(_host, std::to_string( _port ),
                                    boost::asio::ip::tcp::resolver::query::numeric_service);
         boost::asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

         // Try each endpoint until we successfully establish a connection.
         boost::asio::ip::tcp::socket socket(io_service);
         boost::asio::connect(socket, endpoint_iterator);

         // Form the request. We specify the "Connection: close" header so that the
         // server will close the socket after transmitting the response. This will
         // allow us to treat all data up until the EOF as the content.
         boost::asio::streambuf request;
         std::ostream request_stream(&request);
         request_stream << "GET " << _uri << " HTTP/1.0\r\n";
         request_stream << "Host: " << _host << "\r\n";
         request_stream << "Accept: */*\r\n";
         request_stream << "Connection: close\r\n\r\n";

         // Send the request.
         boost::asio::write(socket, request);

         // Read the response status line. The response streambuf will automatically
         // grow to accommodate the entire line. The growth may be limited by passing
         // a maximum size to the streambuf constructor.
         boost::asio::streambuf response;
         boost::asio::read_until(socket, _response_buf, "\r\n");

         // Check that response is OK.
         std::istream response_stream(&_response_buf);
         std::string http_version;
         response_stream >> http_version;
         response_stream >> _status_code;
         std::getline(response_stream, _status_message);
         if (!response_stream || http_version.substr(0, 5) != "HTTP/")
         {
           //std::cout << "Invalid response\n";
           return boost::system::errc::bad_message;
         }
         if (_status_code != 200)
         {
           //std::cout << "Response returned with status code " << _status_code << "\n";
           return boost::asio::error::operation_aborted;
         }

         // Process the response headers.
         std::string header;
         while (std::getline(response_stream, header) && header != "\r")
         {
           //std::cout << header << "\n";
         }

         /*boost::system::error_code error;
         while (boost::asio::read(socket, _response_buf,
               boost::asio::transfer_at_least(1), error))
         {
             std::cerr << &_response_buf;
         }

         if (error != boost::asio::error::eof)
           throw boost::system::system_error(error);*/


         //std::cout << "\n";

         // Write whatever content we already have to output.
         /*if (response.size() > 0)
           std::cout << &response;*/

         // Read until EOF, writing data to output as we go.

       }
       catch (std::exception& e)
       {
         std::cerr << "Exception: " << e.what() << "\n";
         return boost::asio::error::operation_aborted;
       }

     return 0;

 }
