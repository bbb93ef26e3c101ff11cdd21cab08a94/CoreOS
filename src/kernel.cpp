#include <common/types.h>
#include <core/gdt.h>
#include <hardwarecommunication/interrupts.h>
#include <hardwarecommunication/pci.h>
#include <drivers/keyboard.h>
#include <drivers/mouse.h>
#include <drivers/driver.h>
#include <drivers/video/vga.h>
#include <common/memoryoperations.h>

using namespace CoreOS::common;
using namespace CoreOS::core;
using namespace CoreOS::drivers;
using namespace CoreOS::drivers::video;
using namespace CoreOS::hardwarecommunication;

void printf( char* str )
{
    /* video card will display caracters stored at 0xb8000 */
    static uint16_t* VideoMemory = ( uint16_t* )0xb8000;
    static uint8_t x = 0, y = 0;

    for ( int i = 0; str[ i ] != '\0'; ++i )
    {
        switch ( str[ i ] )
        {
            case '\n':
                x = 0;
                y++;
                break;
            default:
                VideoMemory[ 80 * y + x ] = ( VideoMemory[ 80 * y + x ] & 0xFF00 ) | str[ i ];
                x++;
                break;
        }

        if ( x >= 80 )
        {
            x = 0;
            y++;
        }

        if ( y >= 25 )
        {
            for ( y = 0; y < 25; y++ )
                for ( x = 0; x < 80; x++ )
                    VideoMemory[ 80 * y + x ] = ( VideoMemory[ 80 * y + x ] & 0xFF00 ) | ' ';
            x = 0;
            y = 0;
        }
    }
}

void printfHex( uint8_t uKey )
{
    char* foo = "00";
    char* cHex = "0123456789ABCDEF";
    foo[ 0 ] = cHex[ ( uKey >> 4 ) & 0xF ];
    foo[ 1 ] = cHex[ uKey & 0xF ];
    printf( foo );
}

class PrintfKeyboardEventHandler : public KeyboardEventHandler
{
    public:
        void OnKeyDown( char c )
        {
            char* foo = " ";
            foo[ 0 ] = c;
            printf( foo );
        }
};

class MouseToConsole : public MouseEventHandler
{
    int8_t x, y;
    public:
        MouseToConsole( )
        {
            uint16_t* VideoMemory = ( uint16_t* )0xb8000;
            x = 40;
            y = 12;
            /* show the cursor */
            VideoMemory[ 80 * y + x ] = ( VideoMemory[ 80 * y + x ] & 0x0F00 ) << 4
                                      | ( VideoMemory[ 80 * y + x ] & 0xF000 ) >> 4
                                      | ( VideoMemory[ 80 * y + x ] & 0x00FF );        
        }
        
        virtual void OnMouseMove( int xOffset, int yOffset )
        {
            static uint16_t* VideoMemory = ( uint16_t* )0xb8000;
            VideoMemory[ 80 * y + x ] = ( VideoMemory[ 80 * y + x ] & 0x0F00 ) << 4
                                      | ( VideoMemory[ 80 * y + x ] & 0xF000 ) >> 4
                                      | ( VideoMemory[ 80 * y + x ] & 0x00FF );

            /* make sure mouse position doesen't run out of screen bounds */
            x += xOffset;
            if ( x >= 80 ) x = 79;
            if ( x < 0 ) x = 0;

            y += yOffset;
            if ( y >= 25 ) y = 24;
            if ( y < 0 ) y = 0;

            VideoMemory[ 80 * y + x ] = ( VideoMemory[ 80 * y + x ] & 0x0F00 ) << 4
                                      | ( VideoMemory[ 80 * y + x ] & 0xF000 ) >> 4
                                      | ( VideoMemory[ 80 * y + x ] & 0x00FF );
        }
};

typedef void ( *Constructor )( ); /* define what Constructor is */
extern "C" Constructor StartConstructors;
extern "C" Constructor EndConstructors;
extern "C" void CallConstructors( )
{
    for ( Constructor* i = &StartConstructors; i != &EndConstructors; i++ )
        ( *i )( ); /* Initialize the constructors */
}

extern "C" void KernelMain( const void* MultibootStructure, uint32_t MultibootMagic )
{
    printf( "[ OK ] Hello World!\n" );
    printf( "[ OK ] Setting up GDT\n" );
    GlobalDescriptorTable gdt;
    printf( "[ OK ] Setting up IDT\n" );
    InterruptManager Interrupts( 0x20, &gdt );

    printf( "[ OK ] Initializing Hardware: Stage 1\n" );
    DriverManager driverManager;
    
    printf( "[ OK ] Setting up Keyboard Driver\n" );
    PrintfKeyboardEventHandler KeyboardHandler;
    KeyboardDriver Keyboard( &Interrupts, &KeyboardHandler );
    driverManager.AddDriver( &Keyboard );

    printf( "[ OK ] Setting up Mouse Driver\n" );
    MouseToConsole MouseHandler;
    MouseDriver Mouse( &Interrupts, &MouseHandler );
    driverManager.AddDriver( &Mouse );

    printf( "[ OK ] Setting up PCI Controller\n" );
    PeripheralComponentInterconnectController PCIController;
    PCIController.SelectDrivers( &driverManager, &Interrupts );
    VideoGraphicsArray vga;

    printf( "[ OK ] Initializing Hardware: Stage 2\n" );
    printf( "[ OK ] Activating all drivers\n" );
    driverManager.ActivateAll( );

    printf( "[ OK ] Initializing Hardware: Stage 3\n" );
    printf( "[ OK ] Activating interrupts\n" );
    Interrupts.Activate( );
    printf( "[ OK ] Type something using the keyboard: " );
    
    /*
    vga.SetMode( 320, 200, 8 );
    for( int32_t y = 0; y < 200; y++ )
        for( int32_t x = 0; x < 320; x++ )
            vga.PutPixel( x, y, 0x00, 0x00, 0xA8 );
    */

    for ( ;; );
}