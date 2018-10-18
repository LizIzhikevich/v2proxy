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

    static map< string, vector< string > > resource_list_;
    static map< string, vector< CloudResource > > cloud_resource_list_;

    const size_t INVOKE_LIMIT = 2;

    int get_total_resource_count();

public:
    CloudResourceList() {}
    virtual ~CloudResourceList() {}
    
    bool invoke_resource( const std::string & resource_type );

    void push_id_list( std::string & resource_name, std::string & instance_id );
    void terminate_all_ec2s();

    void calculate_cost();

};
#endif
