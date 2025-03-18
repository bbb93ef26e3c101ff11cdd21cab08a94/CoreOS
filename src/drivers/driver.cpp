#include <drivers/driver.h>

using namespace CoreOS::drivers;

Driver::Driver( )
{
}

Driver::~Driver( )
{
}


void Driver::Activate( )
{
}

int Driver::Reset( )
{
    return 0;
}

void Driver::Deactivate( )
{
}

DriverManager::DriverManager( )
{
    numDrivers = 0;
}

void DriverManager::AddDriver( Driver* driver )
{
    Drivers[ numDrivers ] = driver;
    numDrivers++;
}

void DriverManager::ActivateAll( )
{
    /* activates all the drivers in the driver manager */
    for ( int i = 0; i < numDrivers; i++ )
        Drivers[ i ]->Activate( );
}