#include "cloud_resource.hh"

using namespace std;

int CloudResource::resource_counter_ = 0;
int CloudResource::total_cost_ = 0;


/* Constructor */
CloudResource::CloudResource( const std::string & resource_name ) 
    : resource_type_( resource_name ), id_counter_( resource_counter_++ )
    , invoke_( invoke_resource() ) 
{
}

/* Default Constructor */
CloudResource::CloudResource() : CloudResource( "Null" )
{
}

/* Determine whether resource should be invoked */
bool CloudResource::invoke_resource()
{
    return ( this->id_counter_ < INVOKE_LIMIT ); 

}

/* Log behavior */
void CloudResource::record_block()
{

    cerr << "--- " << this->resource_type_ << " " << this->id_counter_ << " blocked because ";
    cerr << "limit exceeded---" << endl;

}

/* Log behavior */
void CloudResource::record_invoke()
{

    cerr << "--- " << this->resource_type_  << " " << this->id_counter_ << " invoked!---" << endl;

}
