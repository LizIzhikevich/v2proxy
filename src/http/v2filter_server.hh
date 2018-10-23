#ifndef V2FILTERSERVER_HH
#define V2FILTERSERVER_HH


#include "http_header.hh"
#include "cloud_resource_list.hh"
#include <list>
#include <algorithm>

using namespace std;

/* log resource data (ids/size/etc) from server responses */
void collect_resource_data( string message, CloudResourceList & resource_list )
{

    /* find instance-id */
    std::size_t first = message.find( "<instanceId>" );
    std::size_t last = message.find( "</instanceId>" );
    std::size_t str_len = strlen( "<instanceId>" );

    /* if instance id found in ec2 response */
    while ( ( first != std::string::npos ) && ( last != std::string::npos ) )
    {

        string instance_id = message.substr( ( first + str_len ), ( last - ( first + str_len ) ) );

        std::string resource_name = "ec2";
            
        /* store the instance id */
        resource_list.assign_resource_id( resource_name, instance_id );

        /* find other instance ids */ 
        first = message.find( "<instanceId>", last + str_len );
        last = message.find( "</instanceId>", last + str_len );

    }

} 

/* Proxy filters */
void v2filter_server( string buffer, CloudResourceList & resource_list )
{

   collect_resource_data( buffer, resource_list );

   //insert other filter calls here

}

#endif /*v2filter*/
