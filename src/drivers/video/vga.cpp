#include <drivers/video/vga.h>

using namespace CoreOS::drivers;
using namespace CoreOS::common;
using namespace CoreOS::drivers::video;

VideoGraphicsArray::VideoGraphicsArray( )
:   MiscPort( 0x3C2 ),
    crtcIndexPort( 0x3D4 ),
    crtcDataPort( 0x3D5 ),
    SequencerIndexPort( 0x3C4 ),
    SequencerDataPort( 0x3C5 ),
    GraphicsControllerIndexPort( 0x3CE ),
    GraphicsControllerDataPort( 0x3CF ),
    AttributeControllerIndexPort( 0x3C0 ),
    AttributeControllerReadPort( 0x3C1 ),
    AttributeControllerWritePort( 0x3C0 ),
    AttributeControllerResetPort( 0x3DA )
{
}

VideoGraphicsArray::~VideoGraphicsArray( )
{
}

/* sends initialization codes to corresponding ports */
void VideoGraphicsArray::WriteRegisters( uint8_t* uRegisters )
{
    /* misc */
    MiscPort.Write( *( uRegisters++ ) ); /* get pointer to register, and increase pointer */

    /* sequencer */
    for ( uint8_t i = 0; i < 5; i++ )
    {
        SequencerIndexPort.Write( i ); /* write data to sequencer index port */
        SequencerDataPort.Write( *( uRegisters++ ) ); /* get pointer to register and increase pointer */
    }

    /* cathode ray tube controller */
    /* unlock crtc, send data to crtc and lock it again */
    /* security measure because CRTs can blow up */
    crtcIndexPort.Write( 0x03 );
    crtcDataPort.Write( crtcDataPort.Read( ) | 0x80 ); /* read the old value and set first bit to 1 */
    crtcIndexPort.Write( 0x11 );
    crtcDataPort.Write( crtcDataPort.Read( ) & ~0x80 ); /* set first bit to 0 */

    uRegisters[ 0x03 ] = uRegisters[ 0x03 ] | 0x80; /* set first bit to 1 */
    uRegisters[ 0x11 ] = uRegisters[ 0x11 ] & ~0x80; /* set first bit to 0 */
    for ( uint8_t i = 0; i < 25; i++ )
    {
        crtcIndexPort.Write( i ); /* write data to crtc index port */
        crtcDataPort.Write( *( uRegisters++ ) ); /* get pointer to register and increase pointer */
    }

    /* graphics controller */
    for ( uint8_t i = 0; i < 9; i++ )
    {
        GraphicsControllerIndexPort.Write( i ); /* write data to graphics controller index port */
        GraphicsControllerDataPort.Write( *( uRegisters++ ) ); /* get pointer to register and increase pointer */
    }

    /* attribute controller */
    for ( uint8_t i = 0; i < 21; i++ )
    {
        AttributeControllerResetPort.Read( ); /* reset attribute controller before we sent data */
        AttributeControllerIndexPort.Write( i ); /* write data to attribute controller index port */
        AttributeControllerWritePort.Write( *( uRegisters++ ) ); /* get pointer to register and increase pointer */
    }

    /* lock CRTC again */
    AttributeControllerResetPort.Read( ); /* reset attribute controller again */
    AttributeControllerIndexPort.Write( 0x20 ); /* add 0x20 to it */
}

bool VideoGraphicsArray::SupportsMode( uint32_t nWidth, uint32_t nHeight, uint32_t ColorDepth )
{
    return nWidth == 320 && nHeight == 200 && ColorDepth == 8;
}

bool VideoGraphicsArray::SetMode( uint32_t nWidth, uint32_t nHeight, uint32_t ColorDepth )
{
    if ( !SupportsMode( nWidth, nHeight, ColorDepth ) )
        return false; 

    /* osdev graphics/modes.c */
    unsigned char g_320x200x256[ ] =
    {
        /* MISC */
            0x63,
        /* SEQ */
            0x03, 0x01, 0x0F, 0x00, 0x0E,
        /* CRTC */
            0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0xBF, 0x1F,
            0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x9C, 0x0E, 0x8F, 0x28, 0x40, 0x96, 0xB9, 0xA3,
            0xFF,
        /* GC */
            0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0F,
            0xFF,
        /* AC */
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
            0x41, 0x00, 0x0F, 0x00, 0x00
    };

    WriteRegisters( g_320x200x256 );
    return true;
}

uint8_t* VideoGraphicsArray::GetFrameBufferSegment( )
{
    /* read data from index number 6 */
    GraphicsControllerIndexPort.Write( 0x06 ); 
    uint8_t SegmentNumber = GraphicsControllerDataPort.Read( ) & ( 3 << 2 );
    switch ( SegmentNumber )
    {
        default:
        case 0 << 2: return ( uint8_t* )0x00000;
        case 1 << 2: return ( uint8_t* )0xA0000;
        case 2 << 2: return ( uint8_t* )0xB0000;
        case 3 << 2: return ( uint8_t* )0xB8000;
    }
}

void VideoGraphicsArray::PutPixel( uint32_t x, uint32_t y, uint8_t ColorIndex )
{
    uint8_t* PixelAddress = GetFrameBufferSegment( ) + 320 * y + x; /* ask GetFrameBufferSegment( ) where to put pixel */
    *PixelAddress = ColorIndex;
}

uint8_t VideoGraphicsArray::GetColorIndex( uint8_t r, uint8_t g, uint8_t b )
{
    if ( r == 0x00, g == 0x00, b == 0xA8 )
        return 0x01;
}

void VideoGraphicsArray::PutPixel( uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b )
{
    PutPixel( x, y, GetColorIndex( r, g, b ) );
}