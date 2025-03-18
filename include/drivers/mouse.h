#ifndef __COREOS__DRIVERS__MOUSE_H
#define __COREOS__DRIVERS__MOUSE_H

#include <common/types.h>
#include <hardwarecommunication/port.h>
#include <drivers/driver.h>
#include <hardwarecommunication/interrupts.h>

namespace CoreOS
{
    namespace drivers
    {
        class MouseEventHandler
        {
            public:
                MouseEventHandler( );

                virtual void OnActivate( );
                virtual void OnMouseDown( CoreOS::common::uint8_t uButton );
                virtual void OnMouseUp( CoreOS::common::uint8_t uButton );
                virtual void OnMouseMove( int x, int y );
        };

        class MouseDriver : public CoreOS::hardwarecommunication::InterruptHandler, public Driver
        {
            CoreOS::hardwarecommunication::Port8Bit DataPort;
            CoreOS::hardwarecommunication::Port8Bit CommandPort;

            CoreOS::common::uint8_t uBuffer[ 3 ];
            CoreOS::common::uint8_t Offset;
            CoreOS::common::uint8_t uButtons;

            MouseEventHandler* MouseHandler;
            public:
                MouseDriver( CoreOS::hardwarecommunication::InterruptManager* interruptManager, MouseEventHandler* MouseHandler );
                ~MouseDriver( );

                virtual CoreOS::common::uint32_t HandleInterrupt( CoreOS::common::uint32_t esp );
                virtual void Activate( );
        };
    }
}

#endif