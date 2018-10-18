#include "cloud_resource.hh"

using namespace std;

/* Constructor */
CloudResource::CloudResource( const std::string & resource_name, int id_counter, bool invoke ) 
    : resource_type_( resource_name ), id_counter_( id_counter )
    , time_at_invoke_( time(0) ), invoke_( invoke ) 
{
}

/* Default Constructor */
CloudResource::CloudResource() : CloudResource( "Null", 0, false )
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

    cerr << "--- " << this->resource_type_ << " " << this->id_counter_ << " blocked because ";
    cerr << "limit exceeded---" << endl;

}

/* Log behavior */
void CloudResource::record_invoke()
{

    cerr << "--- " << this->resource_type_  << " " << this->id_counter_ << " invoked!---" << endl;

}
