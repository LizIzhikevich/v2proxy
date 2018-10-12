#include "cloud_resource.hh"

using namespace std;

int CloudResource::resource_counter = 0;

CloudResource::CloudResource( const std::string & resource_name ) 
    : resource_type( resource_name ), id( resource_counter++ )
    , invoke( invoke_resource( this->id ) ) 
{
}

CloudResource::CloudResource() : CloudResource( "Null" )
{
}

bool CloudResource::invoke_resource( int id )
{
    return ( id < INVOKE_LIMIT ); 

}

void CloudResource::record_block()
{

    cerr << "--- " << this->resource_type << " " << this->id << " blocked because ";
    cerr << "limit exceeded---" <<endl;

}

void CloudResource::record_invoke()
{

    cerr << "--- " << this->resource_type  << " " << this->id << " invoked!---" <<endl;

}