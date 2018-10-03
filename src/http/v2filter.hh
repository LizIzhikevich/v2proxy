#ifndef V2FILTER_HH
#define V2FILTER_HH

using namespace std;
int lambda_counter = 0;


/* Track the number of lambdas that being invoked.
 * Block future invocations that go above the invoke_limit */
void limit_lambdas( HTTPRequestParser * request_parser, int invoke_limit )
{

    if ( not request_parser->empty() ) {
        auto message = request_parser->front().str();

	/* identify lambda invokes in http message */
        if ( message.find("lambda") != std::string::npos ) {

	    /* remove invocations that go above invoke limit */
            if ( lambda_counter >= invoke_limit ) {
       	        request_parser->pop();
            	cerr << "---Lambda " << lambda_counter << " blocked because ";
            	cerr<< "limit exceeded---" <<endl;
            }
	    else {
                cerr << "---Lambda " << lambda_counter << " invoked!---" <<endl;
	    }

	    lambda_counter++;
        }

    }
}


/* Proxy filters */
bool v2filter( HTTPRequestParser request_parser )
{

   limit_lambdas( &request_parser, 5 ); //limit to 5 lambdas per app

   //insert other filter calls here

   return not request_parser.empty();

}

#endif /*v2filter*/
