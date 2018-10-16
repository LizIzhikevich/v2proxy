#include "cloud_resource_list.hh"

map< string, vector< string > > CloudResourceList::resource_list;

/* save IDs */
void CloudResourceList::push_resource_list( std::string & resource_name, std::string & instance_id )
{

    resource_list[ resource_name ].push_back( instance_id );

    cerr << "---Logging instance id: " << instance_id << "---" << endl;

}

void CloudResourceList::terminate_all_ec2s()
{


    /* create a single string of all ids */
    string id_list;
    id_list = accumulate( begin( resource_list[ "ec2" ] ) , end( resource_list[ "ec2" ] ), id_list );

    cerr << " ---Terminating: " << id_list << "---" << endl;

    vector< string > terminate_command { "/home/liz/.local/bin/aws", "ec2", "terminate-instances", 
                                            "--instance-ids" };

    /* insert all terminating ids */
    terminate_command.insert( terminate_command.end(), resource_list[ "ec2" ].begin(), resource_list[ "ec2" ].end() ); 
    /* turn off cert checking */
    terminate_command.push_back( "--no-verify-ssl" );

    //ezexec( terminate_command ); /* Currently this kills the shell */
    // run( terminate_command ); runtime_error: ChildProcess constructed in multi-threaded program

}

/* TODO: Calculate total cost of resources */
void CloudResourceList::calculate_cost()
{
}
