#ifndef CLOUD_RESOURCE_HH
#define CLOUD_RESOURCE_HH

#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <stdio.h>
#include <stdlib.h>


using namespace std;

class CloudResource 
{
private:

    /* member fields */
    static int total_cost_;
    std::string resource_type_;
    int id_counter_;
    static int resource_counter_;
    const int INVOKE_LIMIT = 2;

    /* methods */
    bool invoke_resource();

public:

    /* member fields */
    bool invoke_;

    /* Constructors */
    CloudResource();
    CloudResource( const std::string & resource_name );

    /* getters */
    bool get_invoke();

    /* logging */
    void record_block();
    void record_invoke();

};


#endif /* CLOUD_RESOURCE_HH */
