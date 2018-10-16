#ifndef CLOUD_RESOURCE_LIST
#define CLOUD_RESOURCE_LIST

//#include "cloud_resource.hh"
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <numeric>
#include <thread>
#include "system_runner.hh"

using namespace std;

//template <class CloudResource>
class CloudResourceList
{

private:
    static map< string, vector< string > > resource_list;

public:
    CloudResourceList() {}
    virtual ~CloudResourceList() {}
    
    void push_resource_list( std::string & resource_name, std::string & instance_id );
    void terminate_all_ec2s();

    void calculate_cost();

};
#endif
