#include <hardwarecommunication/interrupts.h>

using namespace CoreOS::common;
using namespace CoreOS::core;
using namespace CoreOS::hardwarecommunication;

void printf( char* str );
void printfHex( uint8_t );

InterruptManager::GateDescriptor InterruptManager::InterruptDescriptorTable[ 256 ];
InterruptManager* InterruptManager::ActiveInterruptManager = 0;

InterruptHandler::InterruptHandler( InterruptManager* interruptManager, uint8_t InterruptNumber )
{
    this->InterruptNumber = InterruptNumber;
    this->interruptManager = interruptManager;
    interruptManager->Handlers[ InterruptNumber ] = this;
}

InterruptHandler::~InterruptHandler( )
{
    if ( interruptManager->Handlers[ InterruptNumber ] == this )
        interruptManager->Handlers[ InterruptNumber ] = 0;
}

uint32_t InterruptHandler::HandleInterrupt( uint32_t esp )
{   
    return esp;
}

void InterruptManager::SetInterruptDescriptorTableEntry(
    uint8_t InterruptNumber,
    uint16_t CodeSegment,
    void ( *Handler )( ),
    uint8_t DescriptorPrivilegeLevel, 
    uint8_t DescriptorType )
{
    const uint8_t IDT_DESC_PRESENT = 0x80;
    InterruptDescriptorTable[ InterruptNumber ].HandlerAddressLowBits = ( ( uint32_t )Handler ) & 0xFFFF;
    InterruptDescriptorTable[ InterruptNumber ].HandlerAddressHighBits = ( ( ( uint32_t )Handler ) >> 16 ) & 0xFFFF;
    InterruptDescriptorTable[ InterruptNumber ].GDTCodeSegmentSelector = CodeSegment;
    InterruptDescriptorTable[ InterruptNumber ].uAccess = IDT_DESC_PRESENT | DescriptorType | ( ( DescriptorPrivilegeLevel & 3 ) << 5 );
    InterruptDescriptorTable[ InterruptNumber ].uReserved = 0;
}

InterruptManager::InterruptManager( uint16_t uHardwareInterruptOffset, GlobalDescriptorTable* globalDescriptorTable )
:   ProgrammableInterruptControllerMasterCommandPort( 0x20 ),
    ProgrammableInterruptControllerMasterDataPort( 0x21 ),
    ProgrammableInterruptControllerSlaveCommandPort( 0xA0 ),
    ProgrammableInterruptControllerSlaveDataPort( 0xA1 )
{
    this->uHardwareInterruptOffset = uHardwareInterruptOffset;
    uint32_t CodeSegment = globalDescriptorTable->CodeSegmentSelector( );

    /* Set all the entry's to Ignore */
    const uint8_t IDT_INTERRUPT_GATE = 0xE;
    for ( uint8_t i = 255; i > 0; --i )
    {
        SetInterruptDescriptorTableEntry( i, CodeSegment, &InterruptIgnore, 0, IDT_INTERRUPT_GATE );
        Handlers[ i ] = 0;
    }
    SetInterruptDescriptorTableEntry( 0, CodeSegment, &InterruptIgnore, 0, IDT_INTERRUPT_GATE );
    Handlers[ 0 ] = 0;

    SetInterruptDescriptorTableEntry( 0x00, CodeSegment, &HandleException0x00, 0, IDT_INTERRUPT_GATE );
    SetInterruptDescriptorTableEntry( 0x01, CodeSegment, &HandleException0x01, 0, IDT_INTERRUPT_GATE );
    SetInterruptDescriptorTableEntry( 0x02, CodeSegment, &HandleException0x02, 0, IDT_INTERRUPT_GATE );
    SetInterruptDescriptorTableEntry( 0x03, CodeSegment, &HandleException0x03, 0, IDT_INTERRUPT_GATE );
    SetInterruptDescriptorTableEntry( 0x04, CodeSegment, &HandleException0x04, 0, IDT_INTERRUPT_GATE );
    SetInterruptDescriptorTableEntry( 0x05, CodeSegment, &HandleException0x05, 0, IDT_INTERRUPT_GATE );
    SetInterruptDescriptorTableEntry( 0x06, CodeSegment, &HandleException0x06, 0, IDT_INTERRUPT_GATE );
    SetInterruptDescriptorTableEntry( 0x07, CodeSegment, &HandleException0x07, 0, IDT_INTERRUPT_GATE );
    SetInterruptDescriptorTableEntry( 0x08, CodeSegment, &HandleException0x08, 0, IDT_INTERRUPT_GATE );
    SetInterruptDescriptorTableEntry( 0x09, CodeSegment, &HandleException0x09, 0, IDT_INTERRUPT_GATE );
    SetInterruptDescriptorTableEntry( 0x0A, CodeSegment, &HandleException0x0A, 0, IDT_INTERRUPT_GATE );
    SetInterruptDescriptorTableEntry( 0x0B, CodeSegment, &HandleException0x0B, 0, IDT_INTERRUPT_GATE );
    SetInterruptDescriptorTableEntry( 0x0C, CodeSegment, &HandleException0x0C, 0, IDT_INTERRUPT_GATE );
    SetInterruptDescriptorTableEntry( 0x0D, CodeSegment, &HandleException0x0D, 0, IDT_INTERRUPT_GATE );
    SetInterruptDescriptorTableEntry( 0x0E, CodeSegment, &HandleException0x0E, 0, IDT_INTERRUPT_GATE );
    SetInterruptDescriptorTableEntry( 0x0F, CodeSegment, &HandleException0x0F, 0, IDT_INTERRUPT_GATE );
    SetInterruptDescriptorTableEntry( 0x10, CodeSegment, &HandleException0x10, 0, IDT_INTERRUPT_GATE );
    SetInterruptDescriptorTableEntry( 0x11, CodeSegment, &HandleException0x11, 0, IDT_INTERRUPT_GATE );
    SetInterruptDescriptorTableEntry( 0x12, CodeSegment, &HandleException0x12, 0, IDT_INTERRUPT_GATE );
    SetInterruptDescriptorTableEntry( 0x13, CodeSegment, &HandleException0x13, 0, IDT_INTERRUPT_GATE );
    SetInterruptDescriptorTableEntry( 0x14, CodeSegment, &HandleException0x14, 0, IDT_INTERRUPT_GATE );
    
    SetInterruptDescriptorTableEntry( uHardwareInterruptOffset + 0x00, CodeSegment, &HandleInterruptRequest0x00, 0, IDT_INTERRUPT_GATE ); /* PIC timer */
    SetInterruptDescriptorTableEntry( uHardwareInterruptOffset + 0x01, CodeSegment, &HandleInterruptRequest0x01, 0, IDT_INTERRUPT_GATE ); /* keyboard */
    SetInterruptDescriptorTableEntry( uHardwareInterruptOffset + 0x02, CodeSegment, &HandleInterruptRequest0x02, 0, IDT_INTERRUPT_GATE );
    SetInterruptDescriptorTableEntry( uHardwareInterruptOffset + 0x03, CodeSegment, &HandleInterruptRequest0x03, 0, IDT_INTERRUPT_GATE );
    SetInterruptDescriptorTableEntry( uHardwareInterruptOffset + 0x04, CodeSegment, &HandleInterruptRequest0x04, 0, IDT_INTERRUPT_GATE );
    SetInterruptDescriptorTableEntry( uHardwareInterruptOffset + 0x05, CodeSegment, &HandleInterruptRequest0x05, 0, IDT_INTERRUPT_GATE );
    SetInterruptDescriptorTableEntry( uHardwareInterruptOffset + 0x06, CodeSegment, &HandleInterruptRequest0x06, 0, IDT_INTERRUPT_GATE );
    SetInterruptDescriptorTableEntry( uHardwareInterruptOffset + 0x07, CodeSegment, &HandleInterruptRequest0x07, 0, IDT_INTERRUPT_GATE ); /* floppy disk */
    SetInterruptDescriptorTableEntry( uHardwareInterruptOffset + 0x08, CodeSegment, &HandleInterruptRequest0x08, 0, IDT_INTERRUPT_GATE );
    SetInterruptDescriptorTableEntry( uHardwareInterruptOffset + 0x09, CodeSegment, &HandleInterruptRequest0x09, 0, IDT_INTERRUPT_GATE );
    SetInterruptDescriptorTableEntry( uHardwareInterruptOffset + 0x0A, CodeSegment, &HandleInterruptRequest0x0A, 0, IDT_INTERRUPT_GATE );
    SetInterruptDescriptorTableEntry( uHardwareInterruptOffset + 0x0B, CodeSegment, &HandleInterruptRequest0x0B, 0, IDT_INTERRUPT_GATE );
    SetInterruptDescriptorTableEntry( uHardwareInterruptOffset + 0x0C, CodeSegment, &HandleInterruptRequest0x0C, 0, IDT_INTERRUPT_GATE );
    SetInterruptDescriptorTableEntry( uHardwareInterruptOffset + 0x0D, CodeSegment, &HandleInterruptRequest0x0D, 0, IDT_INTERRUPT_GATE );
    SetInterruptDescriptorTableEntry( uHardwareInterruptOffset + 0x0E, CodeSegment, &HandleInterruptRequest0x0E, 0, IDT_INTERRUPT_GATE );
    SetInterruptDescriptorTableEntry( uHardwareInterruptOffset + 0x0F, CodeSegment, &HandleInterruptRequest0x0F, 0, IDT_INTERRUPT_GATE );

    /* send initialization control words */
    ProgrammableInterruptControllerMasterCommandPort.Write( 0x11 );
    ProgrammableInterruptControllerSlaveCommandPort.Write( 0x11 );

    /* tell the PIC to add 0x20 to it */
    ProgrammableInterruptControllerMasterDataPort.Write( uHardwareInterruptOffset );
    ProgrammableInterruptControllerSlaveDataPort.Write( uHardwareInterruptOffset + 8 );

    ProgrammableInterruptControllerMasterDataPort.Write( 0x04 );
    ProgrammableInterruptControllerSlaveDataPort.Write( 0x02 );
    ProgrammableInterruptControllerMasterDataPort.Write( 0x01 );
    ProgrammableInterruptControllerSlaveDataPort.Write( 0x01 );

    ProgrammableInterruptControllerMasterDataPort.Write( 0x00 );
    ProgrammableInterruptControllerSlaveDataPort.Write( 0x00 );

    /* tell the processor to use this idt */
    InterruptDescriptorTablePointer IDTPointer;
    IDTPointer.uSize = 256 * sizeof( GateDescriptor ) - 1;
    IDTPointer.uBase = ( uint32_t )InterruptDescriptorTable;
    asm volatile( "lidt %0": : "m" ( IDTPointer ) ); 
}

InterruptManager::~InterruptManager( )
{
    Deactivate( );
}

uint16_t InterruptManager::HardwareInterruptOffset( )
{
    return uHardwareInterruptOffset;
}

void InterruptManager::Activate( )
{
    if ( ActiveInterruptManager != 0 )
        ActiveInterruptManager->Deactivate( );

    ActiveInterruptManager = this;
    asm( "sti" ); /* start interrupts */
}

void InterruptManager::Deactivate( )
{
    if ( ActiveInterruptManager == this )
    {
        ActiveInterruptManager = 0;
        asm( "cli" ); /* clear interrupts */
    }
}

uint32_t InterruptManager::HandleInterrupt( uint8_t Interrupt, uint32_t esp )
{
    if ( ActiveInterruptManager != 0 )
        return ActiveInterruptManager->DoHandleInterrupt( Interrupt, esp );
    
    return esp;
}

uint32_t InterruptManager::DoHandleInterrupt( uint8_t Interrupt, uint32_t esp )
{
    if ( Handlers[ Interrupt ] != 0 )
    {
        esp = Handlers[ Interrupt ]->HandleInterrupt( esp );
    }
    else if ( Interrupt != uHardwareInterruptOffset ) /* timer interrupt */
    {
        printf( "UNHANDLED INTERRUPT 0x" );
        printfHex( Interrupt );
    }

    /* hardware interrupts must be acknowledged */
    if ( uHardwareInterruptOffset <= Interrupt && Interrupt < uHardwareInterruptOffset + 16 )
    {
        ProgrammableInterruptControllerMasterCommandPort.Write( 0x20 );
        if ( uHardwareInterruptOffset + 8 <= Interrupt )
            ProgrammableInterruptControllerSlaveCommandPort.Write( 0x20 );
    }

    return esp;
}