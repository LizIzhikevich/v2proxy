#include "cloud_resource.hh"

using namespace std;

int CloudResource::resource_counter = 0;
 int CloudResource::total_cost = 0;


/* Constructor */
CloudResource::CloudResource( const std::string & resource_name ) 
    : resource_type( resource_name ), id( resource_counter++ )
    , invoke( invoke_resource( this->id ) ) 
{
}

/* Default Constructor */
CloudResource::CloudResource() : CloudResource( "Null" )
{
}

/* Determine whether resource should be invoked */
bool CloudResource::invoke_resource( int id )
{
    return ( id < INVOKE_LIMIT ); 

}

/* TODO: Calculate total cost of resources */ 
void CloudResource::calculate_cost()
{

}


/* Log behavior */
void CloudResource::record_block()
{

    cerr << "--- " << this->resource_type << " " << this->id << " blocked because ";
    cerr << "limit exceeded---" <<endl;

}

/* Log behavior */
void CloudResource::record_invoke()
{

    cerr << "--- " << this->resource_type  << " " << this->id << " invoked!---" <<endl;

}
