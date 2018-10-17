#include "cloud_resource.hh"

using namespace std;

int CloudResource::resource_counter_ = 0;
int CloudResource::total_cost_ = 0;


/* Constructor */
CloudResource::CloudResource( const std::string & resource_name ) 
    : resource_type_( resource_name ), id_counter_( resource_counter_++ )
    , invoke_( false ) 
{
}

/* Default Constructor */
CloudResource::CloudResource() : CloudResource( "Null" )
{
}


/* Determine whether resource WAS invoked */
bool CloudResource::get_invoke() 
{
    return invoke_;
}


/* Log behavior */
void CloudResource::record_block()
{

    this->invoke_ = false;

    cerr << "--- " << this->resource_type_ << " " << this->id_counter_ << " blocked because ";
    cerr << "limit exceeded---" << endl;

}

/* Log behavior */
void CloudResource::record_invoke()
{
    this->invoke_ = true;

    cerr << "--- " << this->resource_type_  << " " << this->id_counter_ << " invoked!---" << endl;

}
