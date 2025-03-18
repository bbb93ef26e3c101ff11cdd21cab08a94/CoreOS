#ifndef __COREOS__HARDWARECOMMUNICATION__INTERRUPTS_H
#define __COREOS__HARDWARECOMMUNICATION__INTERRUPTS_H

#include <core/gdt.h>
#include <common/types.h>
#include <hardwarecommunication/port.h>

namespace CoreOS
{
    namespace hardwarecommunication
    {
        class InterruptManager;
        class InterruptHandler
        {
            protected:
                CoreOS::common::uint8_t InterruptNumber;
                InterruptManager* interruptManager;

                InterruptHandler( InterruptManager* interruptManager, CoreOS::common::uint8_t InterruptNumber );
                ~InterruptHandler( );
            public:
                virtual CoreOS::common::uint32_t HandleInterrupt( CoreOS::common::uint32_t esp );
        };

        class InterruptManager
        {
            friend class InterruptHandler;
            protected:
                static InterruptManager* ActiveInterruptManager;
                InterruptHandler* Handlers[ 256 ];

                struct GateDescriptor
                {
                    CoreOS::common::uint16_t HandlerAddressLowBits;
                    CoreOS::common::uint16_t GDTCodeSegmentSelector;
                    CoreOS::common::uint8_t uReserved;
                    CoreOS::common::uint8_t uAccess;
                    CoreOS::common::uint16_t HandlerAddressHighBits;
                } __attribute__( ( packed ) );

                static GateDescriptor InterruptDescriptorTable[ 256 ];
                struct InterruptDescriptorTablePointer
                {
                    CoreOS::common::uint16_t uSize;
                    CoreOS::common::uint32_t uBase;
                } __attribute__( ( packed ) );

                CoreOS::common::uint16_t uHardwareInterruptOffset;
                static void SetInterruptDescriptorTableEntry(
                    CoreOS::common::uint8_t InterruptNumber,
                    CoreOS::common::uint16_t CodeSegment,
                    void ( *Handler )( ),
                    CoreOS::common::uint8_t DescriptorPrivilegeLevel, 
                    CoreOS::common::uint8_t DescriptorType
                );

                static void InterruptIgnore( );

                static void HandleInterruptRequest0x00( );
                static void HandleInterruptRequest0x01( );
                static void HandleInterruptRequest0x02( );
                static void HandleInterruptRequest0x03( );
                static void HandleInterruptRequest0x04( );
                static void HandleInterruptRequest0x05( );
                static void HandleInterruptRequest0x06( );
                static void HandleInterruptRequest0x07( );
                static void HandleInterruptRequest0x08( );
                static void HandleInterruptRequest0x09( );
                static void HandleInterruptRequest0x0A( );
                static void HandleInterruptRequest0x0B( );
                static void HandleInterruptRequest0x0C( );
                static void HandleInterruptRequest0x0D( );
                static void HandleInterruptRequest0x0E( );
                static void HandleInterruptRequest0x0F( );
                static void HandleInterruptRequest0x31( );

                static void HandleException0x00( );
                static void HandleException0x01( );
                static void HandleException0x02( );
                static void HandleException0x03( );
                static void HandleException0x04( );
                static void HandleException0x05( );
                static void HandleException0x06( );
                static void HandleException0x07( );
                static void HandleException0x08( );
                static void HandleException0x09( );
                static void HandleException0x0A( );
                static void HandleException0x0B( );
                static void HandleException0x0C( );
                static void HandleException0x0D( );
                static void HandleException0x0E( );
                static void HandleException0x0F( );
                static void HandleException0x10( );
                static void HandleException0x11( );
                static void HandleException0x12( );
                static void HandleException0x13( );
                static void HandleException0x14( );

                static CoreOS::common::uint32_t HandleInterrupt( CoreOS::common::uint8_t Interrupt, CoreOS::common::uint32_t esp );
                CoreOS::common::uint32_t DoHandleInterrupt( CoreOS::common::uint8_t Interrupt, CoreOS::common::uint32_t esp ); 

                CoreOS::hardwarecommunication::Port8BitSlow ProgrammableInterruptControllerMasterCommandPort;
                CoreOS::hardwarecommunication::Port8BitSlow ProgrammableInterruptControllerMasterDataPort;
                CoreOS::hardwarecommunication::Port8BitSlow ProgrammableInterruptControllerSlaveCommandPort;
                CoreOS::hardwarecommunication::Port8BitSlow ProgrammableInterruptControllerSlaveDataPort;

            public:
                InterruptManager( CoreOS::common::uint16_t uHardwareInterruptOffset, CoreOS::core::GlobalDescriptorTable* globalDescriptorTable );
                ~InterruptManager( );

                CoreOS::common::uint16_t HardwareInterruptOffset( );
                void Activate( );
                void Deactivate( );
        };
    }
}

#endif