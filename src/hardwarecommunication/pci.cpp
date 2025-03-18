#include <hardwarecommunication/pci.h>

using namespace CoreOS::common;
using namespace CoreOS::drivers;
using namespace CoreOS::hardwarecommunication;

void printf( char* str );
void printfHex( uint8_t );

PeripheralComponentInterconnectDeviceDescriptor::PeripheralComponentInterconnectDeviceDescriptor( )
{
}

PeripheralComponentInterconnectDeviceDescriptor::~PeripheralComponentInterconnectDeviceDescriptor( )
{
}

PeripheralComponentInterconnectController::PeripheralComponentInterconnectController( )
:   DataPort( 0xCFC ) /* PCI Controller */,
    CommandPort( 0xCF8 )
{
}

PeripheralComponentInterconnectController::~PeripheralComponentInterconnectController( )
{
}

/* read from the PCI Controller */
uint32_t PeripheralComponentInterconnectController::Read( uint16_t uBus, uint16_t uDevice, uint16_t Function, uint32_t RegisterOffset )
{
    /*
    * 31 -> Enable bit
    * 30-24 -> Reserved
    * 23-16 -> Bus number 
    * 15-11 -> Device number
    * 10-8 -> Function number
    * 7-0 -> Register offset
    */
    uint32_t ID = 
        0x1 << 31
        | ( ( uBus & 0xFF ) << 16 )
        | ( ( uDevice & 0x1F ) << 11 )
        | ( ( Function & 0x07 ) << 8 )
        | ( RegisterOffset & 0xFC ); /* cut off the last 2 bits of the number */

    CommandPort.Write( ID ); /* write this ID to command port */
    uint32_t uResult = DataPort.Read( ); /* read the result from data port */
    return uResult >> ( 8 * ( RegisterOffset % 4 ) ); /* return the requested byte */
}

void PeripheralComponentInterconnectController::Write( uint16_t uBus, uint16_t uDevice, uint16_t Function, uint32_t RegisterOffset, uint32_t Value )
{
    uint32_t ID = 
        0x1 << 31
        | ( ( uBus & 0xFF ) << 16 )
        | ( ( uDevice & 0x1F ) << 11 )
        | ( ( Function & 0x07 ) << 8 )
        | ( RegisterOffset & 0xFC ); /* cut off the last 2 bits of the number */

    CommandPort.Write( ID ); /* write this ID to command port */
    DataPort.Write( Value ); /* write value to data port */
}

bool PeripheralComponentInterconnectController::DeviceHasFunctions( uint16_t uBus, uint16_t uDevice )
{
    return Read( uBus, uDevice, 0, 0x0E ) & ( 1 << 7 ); /* read the bit with address 0x0E */
}

void PeripheralComponentInterconnectController::SelectDrivers( DriverManager* driverManager, InterruptManager* Interrupts )
{
    /* iterate over buses */
    for ( int bus = 0; bus < 8; bus++ )
    {
        /* iterate the devices */
        for ( int Device = 0; Device < 32; Device++ )
        {
            /* if the device has functions, numFunction will be 8 else will be 1 */
            int numFunctions = DeviceHasFunctions( bus, Device ) ? 8 : 1; 

            /* iterate the functions */
            for ( int Function = 0; Function < numFunctions; Function++ )
            {
                PeripheralComponentInterconnectDeviceDescriptor dev = GetDeviceDescriptor( bus, Device, Function );

                /* if is no device then vendor ID is 0x0000 */
                if ( dev.VendorID == 0x0000 || dev.VendorID == 0xFFFF )
                    continue; 

                /* iterate over base address registers */
                for ( int barNum = 0; barNum < 6; barNum++ )
                {
                    BaseAddressRegister bar = GetBaseAddressRegister( bus, Device, Function, barNum );

                    /* proceed if BAR is really set and is IO type */
                    if ( bar.Address && ( bar.Type == InputOutput ) )
                        dev.PortBase = ( uint32_t )bar.Address; /* set portbase */
                }
                
                Driver* driver = GetDriver( dev, Interrupts );
                if ( driver != 0 )
                    driverManager->AddDriver( driver );

                /* display info */
                printf( "PCI BUS " );
                printfHex( bus & 0xFF );
                
                printf( ", DEVICE " );
                printfHex( Device & 0xFF );

                printf( ", FUNCTION " );
                printfHex( Function & 0xFF );

                printf( " = VENDOR " );
                printfHex( ( dev.VendorID & 0xFF00 ) >> 8 );
                printfHex( dev.VendorID & 0xFF );

                printf( ", DEVICE " );
                printfHex( ( dev.DeviceID & 0xFF00 ) >> 8 );
                printfHex( dev.DeviceID & 0xFF );
                printf( "\n" );
            }
        }
    }
}

BaseAddressRegister PeripheralComponentInterconnectController::GetBaseAddressRegister( uint16_t Bus, uint16_t Device, uint16_t Function, uint16_t Bar )
{
    BaseAddressRegister Result;
    uint32_t headerType = Read( Bus, Device, Function, 0x0E ) & 0x7F; /* reading 0x0E offset */
    int maxBARs = 6 - ( 4 * headerType );
    if ( Bar >= maxBARs ) 
        return Result;

    /* bars have a size of 4 */
    uint32_t BarValue = Read( Bus, Device, Function, 0x10 + 4 * Bar ); /* read 0x10 offset */
    Result.Type = ( BarValue & 0x1 ) ? InputOutput : MemoryMapping; /* examine the last bit which tells usif it's IO type or a MemoryMapping type */
    uint32_t temp;

    if ( Result.Type == MemoryMapping ) 
    {
        switch ( ( BarValue >> 1 ) & 0x3 )
        {
            case 0: /* 32-bit Mode */
            case 1: /* 20-bit Mode */
            case 2: /* 64-bit Mode */
                break;
        }
    }
    else /* InputOutput */
    {
        Result.Address = ( uint8_t* )( BarValue & ~0x3 ); /* set the address to bar value and cancel last 2 bits */
        Result.bPrefetchable = ( ( BarValue >> 3 ) & 0x01 ) == 0x01;
    }
    return Result;
}

Driver* PeripheralComponentInterconnectController::GetDriver( PeripheralComponentInterconnectDeviceDescriptor dev, InterruptManager* Interrupts )
{
    switch ( dev.VendorID )
    {
        case 0x1022: /* AMD */
            switch ( dev.DeviceID )
            {
                case 0x2000: /* am79c973 */
                    printf( "AMD am79c973 " );
                    break;
            }
            break;

        case 0x8086: /* Intel */
            break;
    }

    switch ( dev.ClassID )
    {
        case 0x03: /* graphics */
            switch ( dev.SubclassID )
            {
                case 0x00: /* VGA */
                    printf( "VGA " );
                    break;

                case 0x01: /* XGA */
                    printf( "XGA " );
                    break;
            }
            break;
    }

    return 0;
}

PeripheralComponentInterconnectDeviceDescriptor PeripheralComponentInterconnectController::GetDeviceDescriptor( uint16_t Bus, uint16_t Device, uint16_t Function )
{
    PeripheralComponentInterconnectDeviceDescriptor Result;
    Result.uBus = Bus;
    Result.uDevice = Device;
    Result.uFunction = Function;

    Result.VendorID = Read( Bus, Device, Function, 0x00 );
    Result.DeviceID = Read( Bus, Device, Function, 0x02 );

    Result.ClassID = Read( Bus, Device, Function, 0x0B );
    Result.SubclassID = Read( Bus, Device, Function, 0x0A );
    Result.InterfaceID = Read( Bus, Device, Function, 0x09 );

    Result.revision = Read( Bus, Device, Function, 0x08 );
    Result.Interrupt = Read( Bus, Device, Function, 0x3C );
    return Result;
}