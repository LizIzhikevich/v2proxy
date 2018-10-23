#ifndef V2FILTERCLIENT_HH
#define V2FILTERCLIENT_HH

#include "http_header.hh"
#include "cloud_resource.hh"
#include "cloud_resource_list.hh"
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
		    std::map< string, std::vector<std::string> > resource_keywords, 
                    CloudResourceList & cloud_resource_list )
{

    if ( not request_parser.empty() ) {
        auto message = request_parser.front();
        auto message_str = message.str();

	/* identify which resource in http message */
	for ( const auto &resource : resource_keywords ) {
        
            auto resource_type = resource.first;
            auto keywords = resource.second;
            
            /* if all keywords are present in message */
	    if ( std::all_of( keywords.begin(), keywords.end(), 
                [message]( string s ) { return ( message.str().find(s) != std::string::npos ); } ) )
            {

                /* TODO: Currently assuming that 1 invocation message = 1 resource invoke 
                 * This is actually not the case when setting MaxCount > 1 in invoke message...
                 * will need to address by parsing message for max count...(?)
                 * and then perhaps change max count to be < limit ...(?)
                 */

                bool invoked = false;

                /* identify max amount of ec2s being invoked */
                if ( resource_type == "ec2" )
                {

                    /* Search the message for the amount of invocations */
                    std::size_t first = message_str.find( "MaxCount=" );
                    std::size_t last = message_str.find( "POST" );
                    std::size_t str_len = strlen( "MaxCount=" );

                    if ( ( first != std::string::npos ) && ( last != std::string::npos ) )
                    {

                        int count_requested = stoi( message_str.substr( ( first + str_len ), 
                            ( last - ( first + str_len ) ) ) );


                        /* attempt to invoke all at once */
                        int successful_invokes = cloud_resource_list.invoke_many_resources( resource_type,
                                                  count_requested );
                
                        /* modify http request to amt of approved invocations if necessary */
                        if ( successful_invokes <= count_requested ) 
                        {
                            invoked = true;

                            auto new_request = message_str.replace( ( first + str_len ), 
                                            sizeof( std::to_string( count_requested ) ),
                                            std::to_string( successful_invokes ) );

                            cerr << new_request << endl;
		            /* never deliver original to server */
		            request_parser.pop();
                    
                            /* deliver modified request */
                            request_parser.parse( new_request );
                            
                            invoked = true;

                        }
                    }
                    else {

                        throw runtime_error( "EC2 RunInstances request is abnormal. Please inspect" );

                    }

                }
                /* Not EC2 */
                else {

                    /* Attempt to invoke cloud resource object */
                    invoked = cloud_resource_list.invoke_resource( resource_type );
                }

	        /* remove invocations that go above invoke limit */
                if ( not invoked) {

		    /* never deliver to server */
		    request_parser.pop();


                    /* TODO: Unable to parse response (no element found: line 1, column 0), invalid XML received
                     * this ec2 when blocked err might be coming from somewhere here... */
		    /* spoof server response */
		    response_parser.new_request_arrived(message);
                    /* TODO: Play around with code. 429 (aws appropriate) causes re-invokes on boto3 part */
		    response_parser.parse( get_canned_response( 405, message ) ); 

                    //cloud_resource_list.terminate_all_ec2s();
  
                }
            }

	    //cerr << message.str() << endl;

        }
    
    }
} /* end of limit_resource */

/* Proxy filters */
bool v2filter_client( HTTPRequestParser & request_parser, HTTPResponseParser & response_parser, 
                    CloudResourceList & cloud_resource_list )
{

    std::map< string, std::vector<std::string> > resource_keywords;
    resource_keywords[ "lambda" ] = { "lambda" };
    resource_keywords[ "ec2" ] = { "ec2", "RunInstances" };

   /* limit to 5 lambdas per app */
   limit_resource( request_parser, response_parser, resource_keywords, cloud_resource_list );

   //insert other filter calls here

   return not request_parser.empty();

}

#endif /*v2filter*/
