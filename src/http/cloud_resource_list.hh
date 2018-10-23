#ifndef CLOUD_RESOURCE_LIST
#define CLOUD_RESOURCE_LIST

#include "cloud_resource.hh"
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <numeric>
#include <thread>
#include "system_runner.hh"
#include <mutex>


using namespace std;

//template <class CloudResource>
class CloudResourceList
{

private:

    static map< string, vector< CloudResource > > cloud_resource_list_;

    const size_t INVOKE_LIMIT = 2;

    int get_total_resource_count();

public:
    CloudResourceList() {}
    virtual ~CloudResourceList() {}
    
    bool invoke_resource( const std::string & resource_type );
    int invoke_many_resources( const std::string & resource_type, int count_requested );

    void assign_resource_id( std::string & resource_name, std::string & instance_id );

    void terminate_all_ec2s();

    void print_resources();

    void calculate_cost();

};
#endif
