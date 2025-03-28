#ifndef __COREOS__DRIVERS__DRIVER_H
#define __COREOS__DRIVERS__DRIVER_H

namespace CoreOS
{
    namespace drivers
    {
        class Driver
        {
            public:
                Driver( );
                ~Driver( );

                virtual void Activate( );
                virtual int Reset( );
                virtual void Deactivate( );
        };

        class DriverManager
        {
            private:
                Driver* Drivers[ 265 ];
                int numDrivers;

            public:
                DriverManager( );
                void AddDriver( Driver* driver );
                void ActivateAll( );
        };
    }
}

#endif