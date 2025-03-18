#ifndef __COREOS__COMMON__MEMORYOPERATIONS_H
#define __COREOS__COMMON__MEMORYOPERATIONS_H

#include <common/types.h>

namespace CoreOS
{
    namespace common
    {
        class MemoryOperations
        {
            public:
                static void* memcpy( void* Destination, void* Source, common::uint32_t nSize );
                static void* memset( void* ptr, char Value, common::uint32_t nSize );
                static void* strlen( char* str );
        };
    }
}

#endif