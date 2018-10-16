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
    static int total_cost;
    std::string resource_type;
    int id_counter;
    static int resource_counter;
    const int INVOKE_LIMIT = 5;

    /* methods */
    bool invoke_resource( int );

public:

    /* member fields */
    bool invoke;

    /* Constructors */
    CloudResource();
    virtual ~CloudResource() {}  
    CloudResource( const std::string & str );

    /* logging */
    void record_block();
    void record_invoke();

};


#endif /* CLOUD_RESOURCE_HH */
