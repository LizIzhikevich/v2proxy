#include "cloud_resource_list.hh"

map< string, vector< string > > CloudResourceList::resource_list_;
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
        curr.record_invoke();
        return true;

    }
    /* exceeding invoke limit */
    else { 
        
        CloudResource curr( resource_type, get_total_resource_count(), false );

        cloud_resource_list_[ "blocked" ].push_back( curr );
        curr.record_block();
        return false;

    }    

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


/* save IDs */
void CloudResourceList::push_id_list( std::string & resource_name, std::string & instance_id )
{

    resource_list_[ resource_name ].push_back( instance_id );

    cerr << "---Logging instance id: " << instance_id << "---" << endl;

}

void CloudResourceList::terminate_all_ec2s()
{


    /* create a single string of all ids */
    string id_list;
    id_list = accumulate( begin( resource_list_[ "ec2" ] ) , end( resource_list_[ "ec2" ] ), id_list );

    cerr << " ---Terminating: " << id_list << "---" << endl;

    vector< string > terminate_command { "/home/liz/.local/bin/aws", "ec2", "terminate-instances", 
                                            "--instance-ids" };

    /* insert all terminating ids */
    terminate_command.insert( terminate_command.end(), resource_list_[ "ec2" ].begin(), resource_list_[ "ec2" ].end() ); 
    /* turn off cert checking */
    terminate_command.push_back( "--no-verify-ssl" );

    //ezexec( terminate_command ); /* Currently this kills the shell */
    // run( terminate_command ); runtime_error: ChildProcess constructed in multi-threaded program

}

/* TODO: Calculate total cost of resources */
void CloudResourceList::calculate_cost()
{
}
