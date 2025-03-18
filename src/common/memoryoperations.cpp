#include <common/memoryoperations.h>

using namespace CoreOS::common;

void* MemoryOperations::memcpy( void* Destination, void* Source, uint32_t nSize )
{
    char* _Destination = ( char* )Destination;
    char* _Source = ( char* )Source;

    for ( int32_t i = 0; i < nSize; i++ )
    {
        _Destination[ i ] = _Source[ i ];
    }

    return _Destination;
}

void* MemoryOperations::memset( void* ptr, char Value, uint32_t nSize )
{
    unsigned char* _ptr = ( unsigned char* )ptr;

    for ( uint32_t i = 0; i < nSize; i++ )
    {
        _ptr[ i ] = ( unsigned char )Value;
    }

    return _ptr;
}

void* MemoryOperations::strlen( char* str )
{
    int32_t len = 0;
    while ( str[ len ] )
    {
        ++len;
    }
    return len;
}