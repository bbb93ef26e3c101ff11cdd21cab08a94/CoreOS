#include <hardwarecommunication/port.h>

using namespace CoreOS::common;
using namespace CoreOS::hardwarecommunication;

Port::Port( uint16_t PortNumber )
{
    this->PortNumber = PortNumber;
}

Port::~Port()
{
}

Port8Bit::Port8Bit( uint16_t PortNumber )
:   Port( PortNumber )
{
}

Port8Bit::~Port8Bit( ) 
{
}

void Port8Bit::Write( uint8_t Data )
{
    __asm__ volatile( "outb %0, %1" : : "a" ( Data ), "Nd" ( PortNumber ) );      
}

uint8_t Port8Bit::Read( )
{
    uint8_t uResult;
    __asm__ volatile( "inb %1, %0" : "=a" ( uResult ) : "Nd" ( PortNumber ) );
    return uResult;
}

Port8BitSlow::Port8BitSlow( uint16_t PortNumber )
:   Port8Bit( PortNumber ) 
{
}

Port8BitSlow::~Port8BitSlow( )
{
}

void Port8BitSlow::Write( uint8_t Data )
{
    __asm__ volatile( "outb %0, %1\njmp 1f\n1: jmp 1f\n1:" : : "a" ( Data ), "Nd" ( PortNumber ) );
}

Port16Bit::Port16Bit( uint16_t PortNumber )
:   Port( PortNumber )
{
}

Port16Bit::~Port16Bit( )
{
}

void Port16Bit::Write( uint16_t Data )
{
    __asm__ volatile( "outw %0, %1" : : "a" ( Data ), "Nd" ( PortNumber ) );
}

uint16_t Port16Bit::Read( )
{
    uint16_t uResult;
    __asm__ volatile( "inw %1, %0" : "=a" ( uResult ) : "Nd" ( PortNumber ) );
    return uResult;
}

Port32Bit::Port32Bit( uint16_t PortNumber )
:   Port( PortNumber )
{
}

Port32Bit::~Port32Bit( )
{
}

void Port32Bit::Write( uint32_t Data )
{
    __asm__ volatile( "outl %0, %1" : : "a" ( Data ), "Nd" ( PortNumber ) );
}

uint32_t Port32Bit::Read( )
{
    uint32_t uResult;
    __asm__ volatile( "inl %1, %0" : "=a" ( uResult ) : "Nd" ( PortNumber ) );
    return uResult;
}