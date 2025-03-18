#include <drivers/mouse.h>

using namespace CoreOS::common;
using namespace CoreOS::drivers;
using namespace CoreOS::hardwarecommunication;

void printf( char* str );

MouseEventHandler::MouseEventHandler( )
{
}

void MouseEventHandler::OnActivate( )
{
}

void MouseEventHandler::OnMouseDown( uint8_t uButton )
{
}

void MouseEventHandler::OnMouseUp( uint8_t uButton )
{
}

void MouseEventHandler::OnMouseMove( int x, int y )
{
}

MouseDriver::MouseDriver( InterruptManager* interruptManager, MouseEventHandler* MouseHandler )
:   InterruptHandler( interruptManager, 0x2C ), /* mouse interrupt */
    DataPort( 0x60 ),
    CommandPort( 0x64 )
{
    this->MouseHandler = MouseHandler;
}

MouseDriver::~MouseDriver( )
{
}

void MouseDriver::Activate( )
{
    Offset = 0;
    uButtons = 0;

    if ( MouseHandler != 0 )
        MouseHandler->OnActivate( );

    CommandPort.Write( 0xA8 ); /* activate interrupts */
    CommandPort.Write( 0x20 ); /* read controller command byte */
    uint8_t uStatus = DataPort.Read( ) | 2;
    CommandPort.Write( 0x60 ); /* set controller command byte */
    DataPort.Write( uStatus ); /* write the status */

    CommandPort.Write( 0xD4 );
    DataPort.Write( 0xF4 );
    DataPort.Read( );
}

uint32_t MouseDriver::HandleInterrupt( uint32_t esp )
{
    uint8_t uStatus = CommandPort.Read( );
    if ( !( uStatus & 0x20 ) )
        return esp;

    uBuffer[ Offset ] = DataPort.Read( );
    Offset = ( Offset + 1 ) % 3;

    if ( Offset == 0 )
    {
        if ( uBuffer[ 1 ] != 0 || uBuffer[ 2 ] != 0 )
        {
            MouseHandler->OnMouseMove( ( int8_t )uBuffer[ 1 ], -( ( int8_t )uBuffer[ 2 ] ) );
        }

        for ( uint8_t i = 0; i < 3; i++ )
        {
            if ( ( uBuffer[ 0 ] & ( 0x1 << i ) ) != ( uButtons & ( 0x1 << i ) ) ) 
            {
                if ( uButtons & ( 0x1 << i ) )
                    MouseHandler->OnMouseUp( i + 1 );
                else
                    MouseHandler->OnMouseDown( i + 1 );
            }
        }
        uButtons = uBuffer[ 0 ];
    }
    return esp;
}