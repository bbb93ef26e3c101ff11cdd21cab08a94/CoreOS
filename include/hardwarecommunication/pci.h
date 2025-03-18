#ifndef __COREOS__HARDWARECOMMUNICATION__PCI_H
#define __COREOS__HARDWARECOMMUNICATION__PCI_H

#include <hardwarecommunication/port.h>
#include <drivers/driver.h>
#include <common/types.h>
#include <hardwarecommunication/interrupts.h>

namespace CoreOS
{
    namespace hardwarecommunication
    {
        enum BaseAddressRegisterType
        {
            MemoryMapping = 0,
            InputOutput = 1 
        };

        class BaseAddressRegister
        {
            public:
                bool bPrefetchable;
                CoreOS::common::uint8_t* Address;
                CoreOS::common::uint32_t Size;
                BaseAddressRegisterType Type;
        };

        class PeripheralComponentInterconnectDeviceDescriptor
        {
            public:
                CoreOS::common::uint32_t PortBase;
                CoreOS::common::uint32_t Interrupt;

                CoreOS::common::uint16_t uBus;
                CoreOS::common::uint16_t uDevice;
                CoreOS::common::uint16_t uFunction;

                CoreOS::common::uint16_t VendorID;
                CoreOS::common::uint16_t DeviceID;

                CoreOS::common::uint8_t ClassID;
                CoreOS::common::uint8_t SubclassID;
                CoreOS::common::uint8_t InterfaceID;
                CoreOS::common::uint8_t revision;

                PeripheralComponentInterconnectDeviceDescriptor( );
                ~PeripheralComponentInterconnectDeviceDescriptor( );
        };

        class PeripheralComponentInterconnectController
        {
            Port32Bit DataPort;
            Port32Bit CommandPort;

            public:
                PeripheralComponentInterconnectController( );
                ~PeripheralComponentInterconnectController( );

                CoreOS::common::uint32_t Read( CoreOS::common::uint16_t uBus, CoreOS::common::uint16_t uDevice, CoreOS::common::uint16_t Function, CoreOS::common::uint32_t RegisterOffset );
                void Write( CoreOS::common::uint16_t uBus, CoreOS::common::uint16_t uDevice, CoreOS::common::uint16_t Function, CoreOS::common::uint32_t RegisterOffset, CoreOS::common::uint32_t Value );
                bool DeviceHasFunctions( CoreOS::common::uint16_t uBus, CoreOS::common::uint16_t uDevice );

                void SelectDrivers( CoreOS::drivers::DriverManager* driverManager, CoreOS::hardwarecommunication::InterruptManager* Interrupts );
                CoreOS::drivers::Driver* GetDriver( PeripheralComponentInterconnectDeviceDescriptor dev, CoreOS::hardwarecommunication::InterruptManager* Interrupts );
                PeripheralComponentInterconnectDeviceDescriptor GetDeviceDescriptor( CoreOS::common::uint16_t Bus, CoreOS::common::uint16_t Device, CoreOS::common::uint16_t uFunction );
                BaseAddressRegister GetBaseAddressRegister( CoreOS::common::uint16_t Bus, CoreOS::common::uint16_t Device, CoreOS::common::uint16_t Function, CoreOS::common::uint16_t Bar );
        };
    }
}

#endif