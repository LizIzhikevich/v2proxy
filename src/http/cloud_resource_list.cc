#include "cloud_resource_list.hh"

map< string, vector< string > > CloudResourceList::resource_list_;
map< string, vector< CloudResource > > CloudResourceList::cloud_resource_list_;

bool CloudResourceList::invoke_resource( CloudResource & resource )
{
 
    /* check if resource can be invoked or needs to be blocked */   
    if ( cloud_resource_list_[ "invoked" ].size() < INVOKE_LIMIT ) 
    {    
        
        cloud_resource_list_[ "invoked" ].push_back( resource );
        resource.record_invoke();
        return true;

    }
    /* exceeding invoke limit */
    else { 
        
        cloud_resource_list_[ "blocked" ].push_back( resource );
        resource.record_block();
        return false;

    }    

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
