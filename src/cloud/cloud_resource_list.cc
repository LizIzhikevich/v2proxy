#include "cloud_resource_list.hh"

map< string, vector< CloudResource > > CloudResourceList::cloud_resource_list_;
std:: mutex cloud_resource_list_mutex;

bool CloudResourceList::invoke_resource( const std::string & resource_type )
{
    /* keep thread safe */ 
    std::lock_guard< std::mutex > lock( cloud_resource_list_mutex );

    /* check if resource about to be created can be invoked or blocked */   
    if ( cloud_resource_list_[ "invoked" ].size() < INVOKE_LIMIT ) 
    {    

        CloudResource curr( resource_type, get_total_resource_count(), true );
        
        cloud_resource_list_[ "invoked" ].push_back( curr );

        return true;

    }
    /* exceeding invoke limit */
    else { 
        
        CloudResource curr( resource_type, get_total_resource_count(), false );

        cloud_resource_list_[ "blocked" ].push_back( curr );
       
        return false;

    }    

}

/* returns how many resources it was able to succesfully invoke */
int CloudResourceList::invoke_many_resources( const std::string & resource_type, int count_requested )
{

    bool invoked = false;
    int invoke_count = 0;

    /* invoke each ec2 individually */
    for ( int i = 0; i < count_requested; i++ )
    {

        invoked = this->invoke_resource( resource_type );

        if ( invoked ) 
        {

            invoke_count += 1;
        
        }

    }

    return invoke_count;

}

int CloudResourceList::get_total_resource_count() 
{

    int total_count = 0;

    for ( auto category : { "invoked", "blocked" } ) { 

        if ( cloud_resource_list_.find( category ) != cloud_resource_list_.end() ) {

            total_count += cloud_resource_list_[ category ].size();

        }

    }

    return total_count;

}


void CloudResourceList::assign_resource_id( std::string & resource_name, std::string & instance_id )
{
    
    for ( auto & resource : cloud_resource_list_[ "invoked" ] ) 
    {

        /* if serial identifier not set yet */
        if ( ( resource.get_type() == resource_name ) && resource.is_serial_identifier_empty() ) 
        {

            resource.set_serial_identifier( instance_id );
            cerr << "---Logging instance id: " << instance_id << "---" << endl;

            return; 

        }

    }
   
    /* Something is not right if we haven't returned yet...more instance ids than
     * allowed cloud resources...
     */
    throw runtime_error( "Failed to assign resource id to available cloud resource." );

}

void CloudResourceList::print_resources() 
{

    bool printed = false;

    cerr << "--- printing final state of cloud resources for this application ---" << endl;

    for ( const auto & category : cloud_resource_list_ ) 
    {
        printed = true;

        cerr << " --- "<< category.first << " --- " << endl;
                
        for ( auto & resource : cloud_resource_list_[ category.first ] )
        {       
            
            resource.print_metadata();
            
        }

    }

    terminate_all_ec2s();
    
    if ( not printed ) 
    {
        cerr << "There are no lingering resources!" << endl;
    }


}


void CloudResourceList::terminate_all_ec2s()
{

    bool printed = false;

    /* for all invoked ec2s, print the command to terminate them */
    if ( cloud_resource_list_.find( "invoked" ) != cloud_resource_list_.end() ) {

        for ( auto & resource : cloud_resource_list_[ "invoked" ] )
        {

            if ( resource.get_type() == "ec2" )
            {

                if ( not printed ) 
                {
                    cerr << "To terminate all EC2 instances run: "<< endl;
                    cerr << "aws ec2 terminate-instances --instance-ids ";

                    printed = true;
                }

                cerr << resource.get_serial_identifier() << " ";
            }

        }

    }

    /* formatting */
    if ( printed ) 
    {
        cerr << endl;
    }
}

/* TODO: Calculate total cost of resources */
void CloudResourceList::calculate_cost()
{
}
