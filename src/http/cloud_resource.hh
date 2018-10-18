#ifndef CLOUD_RESOURCE_HH
#define CLOUD_RESOURCE_HH

#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <ctime>

using namespace std;

class CloudResource 
{
private:

    /* member fields */
    std::string resource_type_;
    int id_counter_;
    time_t time_at_invoke_;
    bool invoke_;

    /* logging */
    void record_block();
    void record_invoke();

public:

    /* Constructors */
    CloudResource();
    CloudResource( const std::string & resource_name, int id_counter, bool invoke );

    bool get_invoke();

};


#endif /* CLOUD_RESOURCE_HH */
