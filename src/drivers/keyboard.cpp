#include <drivers/keyboard.h>

using namespace CoreOS::common;
using namespace CoreOS::drivers;
using namespace CoreOS::hardwarecommunication;

void printf( char* str );
void printfHex( uint8_t );

KeyboardEventHandler::KeyboardEventHandler( )
{
}

void KeyboardEventHandler::OnKeyDown( char )
{
}

void KeyboardEventHandler::OnKeyUp( char )
{
}

KeyboardDriver::KeyboardDriver( InterruptManager* interruptManager, KeyboardEventHandler* KeyboardHandler )
:   InterruptHandler( interruptManager, 0x21 ), /* keyboard interrupt*/
    DataPort( 0x60 ),
    CommandPort( 0x64 )
{
    this->KeyboardHandler = KeyboardHandler;
}

KeyboardDriver::~KeyboardDriver( )
{
}

void KeyboardDriver::Activate( )
{
    while ( CommandPort.Read( ) & 0x1 )
        DataPort.Read( );

    CommandPort.Write( 0xAE ); /* activate interrupts */
    CommandPort.Write( 0x20 ); /* read controller command byte */
    uint8_t uStatus = ( DataPort.Read( ) | 1 ) & ~0x10;
    CommandPort.Write( 0x60 ); /* set controller command byte */
    DataPort.Write( uStatus ); /* write the status */
    DataPort.Write( 0xF4 );
}

uint32_t KeyboardDriver::HandleInterrupt( uint32_t esp )
{
    static bool bShift = false;
    uint8_t uKey = DataPort.Read( );

    if ( KeyboardHandler == 0 )
        return esp;

    if ( uKey < 0x80 )
    {
        switch ( uKey )
        {
            case 0x29: ( bShift ) ? KeyboardHandler->OnKeyDown( '~' ) : KeyboardHandler->OnKeyDown( '`' ); break;
            case 0x02: ( bShift ) ? KeyboardHandler->OnKeyDown( '!' ) : KeyboardHandler->OnKeyDown( '1' ); break;
            case 0x03: ( bShift ) ? KeyboardHandler->OnKeyDown( '@' ) : KeyboardHandler->OnKeyDown( '2' ); break;
            case 0x04: ( bShift ) ? KeyboardHandler->OnKeyDown( '#' ) : KeyboardHandler->OnKeyDown( '3' ); break;
            case 0x05: ( bShift ) ? KeyboardHandler->OnKeyDown( '$' ) : KeyboardHandler->OnKeyDown( '4' ); break;
            case 0x06: ( bShift ) ? KeyboardHandler->OnKeyDown( '%' ) : KeyboardHandler->OnKeyDown( '5' ); break;
            case 0x07: ( bShift ) ? KeyboardHandler->OnKeyDown( '^' ) : KeyboardHandler->OnKeyDown( '6' ); break;
            case 0x08: ( bShift ) ? KeyboardHandler->OnKeyDown( '&' ) : KeyboardHandler->OnKeyDown( '7' ); break;
            case 0x09: ( bShift ) ? KeyboardHandler->OnKeyDown( '*' ) : KeyboardHandler->OnKeyDown( '8' ); break;
            case 0x0A: ( bShift ) ? KeyboardHandler->OnKeyDown( '(' ) : KeyboardHandler->OnKeyDown( '9' ); break;
            case 0x0B: ( bShift ) ? KeyboardHandler->OnKeyDown( ')' ) : KeyboardHandler->OnKeyDown( '0' ); break;
            case 0x0C: ( bShift ) ? KeyboardHandler->OnKeyDown( '_' ) : KeyboardHandler->OnKeyDown( '-' ); break;
            case 0x0D: ( bShift ) ? KeyboardHandler->OnKeyDown( '+' ) : KeyboardHandler->OnKeyDown( '=' ); break;

            case 0x0F: KeyboardHandler->OnKeyDown( ' ' ); break;
            case 0x10: ( bShift ) ? KeyboardHandler->OnKeyDown( 'Q' ) : KeyboardHandler->OnKeyDown( 'q' ); break;
            case 0x11: ( bShift ) ? KeyboardHandler->OnKeyDown( 'W' ) : KeyboardHandler->OnKeyDown( 'w' ); break;
            case 0x12: ( bShift ) ? KeyboardHandler->OnKeyDown( 'E' ) : KeyboardHandler->OnKeyDown( 'e' ); break;
            case 0x13: ( bShift ) ? KeyboardHandler->OnKeyDown( 'R' ) : KeyboardHandler->OnKeyDown( 'r' ); break;
            case 0x14: ( bShift ) ? KeyboardHandler->OnKeyDown( 'T' ) : KeyboardHandler->OnKeyDown( 't' ); break;
            case 0x15: ( bShift ) ? KeyboardHandler->OnKeyDown( 'Y' ) : KeyboardHandler->OnKeyDown( 'y' ); break;
            case 0x16: ( bShift ) ? KeyboardHandler->OnKeyDown( 'U' ) : KeyboardHandler->OnKeyDown( 'u' ); break;
            case 0x17: ( bShift ) ? KeyboardHandler->OnKeyDown( 'I' ) : KeyboardHandler->OnKeyDown( 'i' ); break;
            case 0x18: ( bShift ) ? KeyboardHandler->OnKeyDown( 'O' ) : KeyboardHandler->OnKeyDown( 'o' ); break;
            case 0x19: ( bShift ) ? KeyboardHandler->OnKeyDown( 'P' ) : KeyboardHandler->OnKeyDown( 'p' ); break;
            case 0x1A: ( bShift ) ? KeyboardHandler->OnKeyDown( '{' ) : KeyboardHandler->OnKeyDown( '[' ); break;
            case 0x1B: ( bShift ) ? KeyboardHandler->OnKeyDown( '}' ) : KeyboardHandler->OnKeyDown( ']' ); break;

            case 0x1E: ( bShift ) ? KeyboardHandler->OnKeyDown( 'A' ) : KeyboardHandler->OnKeyDown( 'a' ); break;
            case 0x1F: ( bShift ) ? KeyboardHandler->OnKeyDown( 'S' ) : KeyboardHandler->OnKeyDown( 's' ); break;
            case 0x20: ( bShift ) ? KeyboardHandler->OnKeyDown( 'D' ) : KeyboardHandler->OnKeyDown( 'd' ); break;
            case 0x21: ( bShift ) ? KeyboardHandler->OnKeyDown( 'F' ) : KeyboardHandler->OnKeyDown( 'f' ); break;
            case 0x22: ( bShift ) ? KeyboardHandler->OnKeyDown( 'G' ) : KeyboardHandler->OnKeyDown( 'g' ); break;
            case 0x23: ( bShift ) ? KeyboardHandler->OnKeyDown( 'H' ) : KeyboardHandler->OnKeyDown( 'h' ); break;
            case 0x24: ( bShift ) ? KeyboardHandler->OnKeyDown( 'J' ) : KeyboardHandler->OnKeyDown( 'j' ); break;
            case 0x25: ( bShift ) ? KeyboardHandler->OnKeyDown( 'K' ) : KeyboardHandler->OnKeyDown( 'k' ); break;
            case 0x26: ( bShift ) ? KeyboardHandler->OnKeyDown( 'L' ) : KeyboardHandler->OnKeyDown( 'l' ); break;
            case 0x27: ( bShift ) ? KeyboardHandler->OnKeyDown( ':' ) : KeyboardHandler->OnKeyDown( ';' ); break;
            case 0x28: ( bShift ) ? KeyboardHandler->OnKeyDown( '\'' ) : KeyboardHandler->OnKeyDown( '|' ); break;
            case 0x1C: KeyboardHandler->OnKeyDown( '\n' ); break;  

            case 0x2C: ( bShift ) ? KeyboardHandler->OnKeyDown( 'Z' ) : KeyboardHandler->OnKeyDown( 'z' ); break;
            case 0x2D: ( bShift ) ? KeyboardHandler->OnKeyDown( 'X' ) : KeyboardHandler->OnKeyDown( 'x' ); break;
            case 0x2E: ( bShift ) ? KeyboardHandler->OnKeyDown( 'C' ) : KeyboardHandler->OnKeyDown( 'c' ); break;
            case 0x2F: ( bShift ) ? KeyboardHandler->OnKeyDown( 'V' ) : KeyboardHandler->OnKeyDown( 'v' ); break;
            case 0x30: ( bShift ) ? KeyboardHandler->OnKeyDown( 'B' ) : KeyboardHandler->OnKeyDown( 'b' ); break;
            case 0x31: ( bShift ) ? KeyboardHandler->OnKeyDown( 'N' ) : KeyboardHandler->OnKeyDown( 'n' ); break;
            case 0x32: ( bShift ) ? KeyboardHandler->OnKeyDown( 'M' ) : KeyboardHandler->OnKeyDown( 'm' ); break;
            case 0x33: ( bShift ) ? KeyboardHandler->OnKeyDown( '<' ) : KeyboardHandler->OnKeyDown( ',' ); break;
            case 0x34: ( bShift ) ? KeyboardHandler->OnKeyDown( '>' ) : KeyboardHandler->OnKeyDown( '.' ); break;
            case 0x35: ( bShift ) ? KeyboardHandler->OnKeyDown( '?' ) : KeyboardHandler->OnKeyDown( '/' ); break;        
            case 0x39: KeyboardHandler->OnKeyDown( ' ' ); break;  
            
            case 0x3A: bShift = !bShift; break;                              
            case 0x4A: KeyboardHandler->OnKeyDown( '-' ); break;
            case 0x4E: KeyboardHandler->OnKeyDown( '+' ); break;

            default:
            {
                printf( "KEYBOARD 0x" );
                printfHex( uKey );
                break; 
            }
        }
    }
    
    return esp;
}