/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <thread>
#include <string>
#include <iostream>
#include <arpa/inet.h>
#include <linux/netfilter_ipv4.h>

#include "address.hh"
#include "socket.hh"
#include "system_runner.hh"
#include "http_proxy.hh"
#include "poller.hh"
#include "bytestream_queue.hh"
#include "http_request_parser.hh"
#include "http_response_parser.hh"
#include "file_descriptor.hh"
#include "event_loop.hh"
#include "temp_file.hh"
#include "secure_socket.hh"
#include "backing_store.hh"
#include "exception.hh"
#include "v2filter_client.hh"
#include "v2filter_server.hh"
//#include "cloud_resource_list.hh"

using namespace std;
using namespace PollerShortNames;

HTTPProxy::HTTPProxy( const Address & listener_addr )
    : listener_socket_(),
      server_context_( SERVER ),
      client_context_( CLIENT )
{
    listener_socket_.bind( listener_addr );
    listener_socket_.listen();
}

template <class SocketType>
void HTTPProxy::loop( SocketType & server, SocketType & client, HTTPBackingStore & backing_store, 
                    CloudResourceList & cloud_resource_list)
{
    Poller poller;

    HTTPRequestParser request_parser;
    HTTPResponseParser response_parser;

    const Address server_addr = client.original_dest();

    /* poll on original connect socket and new connection socket to ferry packets */
    /* responses from server go to response parser */
    poller.add_action( Poller::Action( server, Direction::In,
                                       [&] () {
                                           string buffer = server.read();

                                           v2filter_server( buffer, cloud_resource_list );

                                           response_parser.parse( buffer );

                                           return ResultType::Continue;
                                       },
                                       [&] () { return not client.eof(); } ) );

    /* requests from client go to request parser */
    poller.add_action( Poller::Action( client, Direction::In,
                                       [&] () {
                                           string buffer = client.read();

                                           string client_message = v2filter_client( buffer, cloud_resource_list );

                                           /* if some form of client message still being delivered */
                                           if ( client_message != "BLOCKED" ) {

                                               request_parser.parse( client_message );

                                            }
                                            /* spoof server response otherwise */
                                            else {

                                                /* morph message into HTTP_Request */
                                                request_parser.parse( buffer );     
                                                auto client_message = request_parser.front(); 
                                                request_parser.pop();

                                                /* send as response */
                                                response_parser.new_request_arrived( client_message ); 

                                                /* TODO: Play around with code. 
                                                 * 429 (aws appropriate) causes re-invokes on boto3 part */
                                                response_parser.parse( get_canned_response( 405, 
                                                                        client_message ) );
        
                                            }

                                           return ResultType::Continue;
                                       },
                                       [&] () { return not server.eof(); } ) );

    /* completed requests from client are serialized and sent to server */
    poller.add_action( Poller::Action( server, Direction::Out,
                                       [&] () {
                                           server.write( request_parser.front().str() );
                                           response_parser.new_request_arrived( request_parser.front() );
                                           request_parser.pop();
                                           return ResultType::Continue;
                                       },
       				       /* deliver the packet to the server only if it passes the v2 filter */
                                       [&] () { return not request_parser.empty(); } ) );

    /* completed responses from server are serialized and sent to client */
    poller.add_action( Poller::Action( client, Direction::Out,
                                       [&] () {
                                           client.write( response_parser.front().str() );
                                           backing_store.save( response_parser.front(), server_addr );
                                           response_parser.pop();
                                           return ResultType::Continue;
                                       },
                                       [&] () {  return not response_parser.empty(); } ) );

    while ( true ) {
        if ( poller.poll( -1 ).result == Poller::Result::Type::Exit ) {
            return;
        }
    }
}

void HTTPProxy::handle_tcp( HTTPBackingStore & backing_store, CloudResourceList & resource_list )
{
    thread newthread( [&] ( TCPSocket client ) {
            try {
                /* get original destination for connection request */
                Address server_addr = client.original_dest();

                /* create socket and connect to original destination and send original request */
                TCPSocket server;
                server.connect( server_addr );

                if ( server_addr.port() != 443 ) { /* normal HTTP */
                    return loop( server, client, backing_store, resource_list );
                }

                /* handle TLS */
                SecureSocket tls_server( client_context_.new_secure_socket( move( server ) ) );
                tls_server.connect();

                SecureSocket tls_client( server_context_.new_secure_socket( move( client ) ) );
                tls_client.accept();

                loop( tls_server, tls_client, backing_store, resource_list );
            } catch ( const exception & e ) {
                print_exception( e );
            }
        }, listener_socket_.accept() );

    /* don't wait around for the reply */
    newthread.detach();
}

/* register this HTTPProxy's TCP listener socket to handle events with
   the given event_loop, saving request-response pairs to the given
   backing_store (which is captured and must continue to persist) */
void HTTPProxy::register_handlers( EventLoop & event_loop, HTTPBackingStore & backing_store, 
                                CloudResourceList & resource_list )
{
    event_loop.add_simple_input_handler( tcp_listener(),
                                         [&] () {
                                             handle_tcp( backing_store, resource_list );
                                             return ResultType::Continue;
                                         } );
}


/* Spoof server response */
std::string HTTPProxy::get_canned_response( const int status, const HTTPRequest & request )
{

  const static map<int, string> status_messages = {
    { 400, "Bad Request" },
    { 404, "Not Found" },
    { 405, "Method Not Allowed" },
    { 429, "%" },
  };

  HTTPResponse response;
  response.set_request( request );
  response.set_first_line( "HTTP/1.1 " + to_string( status ) + " " + status_messages.at( status ) );
  response.add_header( HTTPHeader{ "Content-Length", "0" } );
  response.add_header( HTTPHeader{ "Content-Type", "text/plain" } );
  response.done_with_headers();
  response.read_in_body( "" );
  assert( response.state() == COMPLETE );

  return response.str();

} 


