#include "cloud_resource.hh"

using namespace std;

int CloudResource::resource_counter = 0;
int CloudResource::total_cost = 0;


/* Constructor */
CloudResource::CloudResource( const std::string & resource_name ) 
    : resource_type( resource_name ), id_counter( resource_counter++ )
    , invoke( invoke_resource( this->id_counter ) ) 
{
}

/* Default Constructor */
CloudResource::CloudResource() : CloudResource( "Null" )
{
}

/* Determine whether resource should be invoked */
bool CloudResource::invoke_resource( int id_counter )
{
    return ( id_counter < INVOKE_LIMIT ); 

}

/* Log behavior */
void CloudResource::record_block()
{

    cerr << "--- " << this->resource_type << " " << this->id_counter << " blocked because ";
    cerr << "limit exceeded---" <<endl;

}

/* Log behavior */
void CloudResource::record_invoke()
{

    cerr << "--- " << this->resource_type  << " " << this->id_counter << " invoked!---" <<endl;

}
