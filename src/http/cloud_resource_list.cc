#include "cloud_resource_list.hh"

using namespace std;

/* save IDs */

map< string, vector< string > > CloudResourceList::resource_list;

void CloudResourceList::push_resource_list( std::string & resource_name, std::string & instance_id )
{

    resource_list[ resource_name ].push_back( instance_id );

    cerr << "---Logging instance id: " << instance_id << "---" << endl;

}

void CloudResourceList::terminate_all_ec2s()
{

    for ( auto id : resource_list[ "ec2" ] ) {

        string terminate_command = "aws ec2 terminate-instances --instance-ids " + id + " --no-verify-ssl";

        if ( system( terminate_command.c_str() ) ) {

            cerr << "---Terminating: " << id << "---" << endl;

        } else {

            cerr << "---Failed to terminate: " << id << "---" << endl;

        }    
    }

}

/* TODO: Calculate total cost of resources */
void CloudResourceList::calculate_cost()
{
}
