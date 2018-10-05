#ifndef V2FILTER_HH
#define V2FILTER_HH

#include <mutex>
#include "socket.hh"
#include "http_header.hh"

using namespace std;
int lambda_counter = 0;

/* Spoof server response */
string get_canned_response( const int status, const HTTPRequest & request )
{
  const static map<int, string> status_messages = {
    { 400, "Bad Request" },
    { 404, "Not Found" },
    { 405, "Method Not Allowed" },
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


/* Track the number of lambdas that being invoked.
 * Block future invocations that go above the invoke_limit */
void limit_lambdas( HTTPRequestParser & request_parser, HTTPResponseParser & response_parser,
		   int invoke_limit )
{

    if ( not request_parser.empty() ) {
        auto message = request_parser.front();


	/* identify lambda invokes in http message */
        if ( message.str().find("lambda") != std::string::npos ) {

	    /* remove invocations that go above invoke limit */
            if ( lambda_counter >= invoke_limit ) {

		/* never deliver to server */
		request_parser.pop();

		/* spoof server response */
		response_parser.new_request_arrived(message);
		response_parser.parse( get_canned_response( 405, message ) );

            	cerr << "---Lambda " << lambda_counter << " blocked because ";
            	cerr << "limit exceeded---" <<endl;
		cerr << message.str() << endl;

            }
	    else {
                cerr << "---Lambda " << lambda_counter << " invoked!---" <<endl;
	    }

	    lambda_counter++;
        }

    }

}


/* Proxy filters */
bool v2filter( HTTPRequestParser & request_parser, HTTPResponseParser & response_parser )
{

   /* limit to 5 lambdas per app */
   limit_lambdas( request_parser, response_parser, 5 );

   //insert other filter calls here

   return not request_parser.empty();

}

#endif /*v2filter*/
