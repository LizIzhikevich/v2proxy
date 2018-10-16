#ifndef V2FILTERCLIENT_HH
#define V2FILTERCLIENT_HH

#include "http_header.hh"
#include "cloud_resource.hh"
#include <list>
#include <algorithm>

using namespace std;

/* Spoof server response */
string get_canned_response( const int status, const HTTPRequest & request )
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


/* Track the number of lambdas that being invoked.
 * Block future invocations that go above the invoke_limit */
void limit_resource( HTTPRequestParser & request_parser, HTTPResponseParser & response_parser,
		    std::map< string, std::vector<std::string> > resource_keywords)
{

    if ( not request_parser.empty() ) {
        auto message = request_parser.front();


	/* identify which resource in http message */
	for ( const auto &resource : resource_keywords ) {
        
            auto resource_type = resource.first;
            auto keywords = resource.second;
            
            /* if all keywords are present in message */
	    if ( std::all_of( keywords.begin(), keywords.end(), 
                [message]( string s) { return ( message.str().find(s) != std::string::npos ); } ) )
            {

                /* Create cloud resource object */
                CloudResource curr( resource_type );

	        /* remove invocations that go above invoke limit */
                if ( not curr.invoke ) {

		    /* never deliver to server */
		    request_parser.pop();

		    /* spoof server response */
		    response_parser.new_request_arrived(message);
		    response_parser.parse( get_canned_response( 429, message ) );

                    curr.record_block();
                    
		    //cerr << message.str() << endl;

                }
	        else {
                    
                    curr.record_invoke();

		    //cerr << message.str() << endl;

	        }

            }

        }
    
    }
} /* end of limit_resource */

/* Proxy filters */
bool v2filter_client( HTTPRequestParser & request_parser, HTTPResponseParser & response_parser )
{

    std::map< string, std::vector<std::string> > resource_keywords;
    resource_keywords[ "lambda" ] = { "lambda" };
    resource_keywords[ "ec2" ] = { "ec2", "RunInstances" };

   /* limit to 5 lambdas per app */
   limit_resource( request_parser, response_parser, resource_keywords );

   //insert other filter calls here

   return not request_parser.empty();

}

#endif /*v2filter*/
