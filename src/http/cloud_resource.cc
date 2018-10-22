#include "cloud_resource.hh"

using namespace std;

/* Constructor */
CloudResource::CloudResource( const std::string & resource_name, int id_counter, bool invoke ) 
    : resource_type_( resource_name ), serial_identifier_( "" ), id_counter_( id_counter )
    , time_at_invoke_( time(0) ), invoke_( invoke ) 
{

    if( invoke ) 
    {

        record_invoke();

    }
    else {

        record_block();
    
    }

}

/* Default Constructor */
CloudResource::CloudResource() : CloudResource( "Null", 0, false )
{
}


void CloudResource::set_serial_identifier( const std::string & identifier ) {

    this->serial_identifier_ = identifier;

}

/* check if serial identifier has already been set */
bool CloudResource::is_serial_identifier_empty() {

    return this->serial_identifier_ == "";

}

std::string CloudResource::get_serial_identifier() {

    if( is_serial_identifier_empty() ) {
  
        return std::to_string(this->id_counter_);

    }

    return this->serial_identifier_; 

}

/* Determine whether resource WAS invoked */
bool CloudResource::get_invoke() 
{
    return invoke_;
}

std::string CloudResource::get_type() {

    return this->resource_type_;

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

void CloudResource::print_metadata()
{

     cerr << this->get_type() << "- id: " << this->get_serial_identifier() << endl;

}
