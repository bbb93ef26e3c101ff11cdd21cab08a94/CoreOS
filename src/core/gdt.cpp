#include <core/gdt.h>

using namespace CoreOS::common;
using namespace CoreOS::core;

GlobalDescriptorTable::GlobalDescriptorTable( )
:   nullSegmentSelector( 0, 0, 0 ),
    unusedSegmentSelector( 0, 0, 0 ),
    codeSegmentSelector( 0, 64 * 1024 * 1024, 0x9A ),
    dataSegmentSelector( 0, 64 * 1024 * 1024, 0x92 )
{
    uint32_t i[ 2 ];
    i[ 1 ] = ( uint32_t )this;
    i[ 0 ] = sizeof( GlobalDescriptorTable ) << 16;
    asm volatile( "lgdt ( %0 )": :"p" ( ( ( uint8_t* ) i ) + 2 ) );
}

GlobalDescriptorTable::~GlobalDescriptorTable( )
{
}

uint16_t GlobalDescriptorTable::DataSegmentSelector( )
{
    return ( uint8_t* )&dataSegmentSelector - ( uint8_t* )this;
}

uint16_t GlobalDescriptorTable::CodeSegmentSelector()
{
    return ( uint8_t* )&codeSegmentSelector - ( uint8_t* )this;
}

GlobalDescriptorTable::SegmentDescriptor::SegmentDescriptor( uint32_t uBase, uint32_t uLimit, uint8_t Type )
{
    uint8_t* uTarget = ( uint8_t* )this;

    if ( uLimit <= 65536 )
    {
        /* 16-bit address space */
        uTarget[ 6 ] = 0x40;
    }
    else
    {
        /*
            32-bit address space
            Now we have to squeeze the (32-bit) limit into 2.5 regiters (20-bit).
            This is done by discarding the 12 least significant bits, but this
            is only legal, if they are all == 1, so they are implicitly still there

            so if the last bits aren't all 1, we have to set them to 1, but this
            would increase the limit (cannot do that, because we might go beyond
            the physical limit or get overlap with other segments) so we have to
            compensate this by decreasing a higher bit (and might have up to
            4095 wasted bytes behind the used memory)
        */

        if ( ( uLimit & 0xFFF ) != 0xFFF )
            uLimit = ( uLimit >> 12 ) - 1;
        else
            uLimit = uLimit >> 12;

        uTarget[ 6 ] = 0xC0;
    }

    /* encode the limit */
    uTarget[ 0 ] = uLimit & 0xFF;
    uTarget[ 1 ] = ( uLimit >> 8 ) & 0xFF;
    uTarget[ 6 ] |= ( uLimit >> 16 ) & 0xF;

    /* encode the base */
    uTarget[ 2 ] = uBase & 0xFF;
    uTarget[ 3 ] = ( uBase >> 8 ) & 0xFF;
    uTarget[ 4 ] = ( uBase >> 16 ) & 0xFF;
    uTarget[ 7 ] = ( uBase >> 24 ) & 0xFF;

    /* type */
    uTarget[ 5 ] = Type;
}

/* look up the pointer */
uint32_t GlobalDescriptorTable::SegmentDescriptor::Base( )
{
    uint8_t* uTarget = ( uint8_t* )this;
    uint32_t uResult = uTarget[ 7 ];
    uResult = ( uResult << 8 ) + uTarget[ 4 ];
    uResult = ( uResult << 8 ) + uTarget[ 3 ];
    uResult = ( uResult << 8 ) + uTarget[ 2 ];

    return uResult;
}

/* look up the limit */
uint32_t GlobalDescriptorTable::SegmentDescriptor::Limit( )
{
    uint8_t* uTarget = ( uint8_t* )this;

    uint32_t uResult = uTarget[ 6 ] & 0xF;
    uResult = ( uResult << 8 ) + uTarget[ 1 ];
    uResult = ( uResult << 8 ) + uTarget[ 0 ];
    if( ( uTarget[ 6 ] & 0xC0 ) == 0xC0 )
        uResult = ( uResult << 12 ) | 0xFFF;

    return uResult;
}