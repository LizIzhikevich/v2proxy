#ifndef CLOUD_RESOURCE_HH
#define CLOUD_RESOURCE_HH

#include <string>
#include <iostream>

class CloudResource 
{
private:

    bool invoke_resource( int );

public:

    /* member fields */
    static int resource_counter;
    const int INVOKE_LIMIT = 5;
    std::string resource_type;
    int id;
    bool invoke;


    /* Constructors */
    CloudResource();
    virtual ~CloudResource() {}  
    CloudResource( const std::string & str );

};
#endif /* CLOUD_RESOURCE_HH */
