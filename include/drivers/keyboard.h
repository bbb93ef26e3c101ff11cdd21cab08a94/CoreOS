#ifndef __COREOS__DRIVERS__KEYBOARD_H
#define __COREOS__DRIVERS__KEYBOARD_H

#include <common/types.h>
#include <hardwarecommunication/interrupts.h>
#include <hardwarecommunication/port.h>
#include <drivers/driver.h>

namespace CoreOS
{
    namespace drivers
    {
        class KeyboardEventHandler
        {
            public:
                KeyboardEventHandler( );

                virtual void OnKeyDown( char );
                virtual void OnKeyUp( char );
        };

        class KeyboardDriver : public CoreOS::hardwarecommunication::InterruptHandler, public Driver
        {
            CoreOS::hardwarecommunication::Port8Bit DataPort;
            CoreOS::hardwarecommunication::Port8Bit CommandPort;

            KeyboardEventHandler* KeyboardHandler;
            public:
                KeyboardDriver( CoreOS::hardwarecommunication::InterruptManager* interruptManager, KeyboardEventHandler* KeyboardHandler );
                ~KeyboardDriver( );

                virtual common::uint32_t HandleInterrupt( common::uint32_t esp );
                virtual void Activate( );
        };
    }
}

#endif