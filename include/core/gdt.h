#ifndef __COREOS__CORE__GDT_H
#define __COREOS__CORE__GDT_H

#include <common/types.h>

namespace CoreOS
{
    namespace core
    {
        class GlobalDescriptorTable
        {
            public:
                class SegmentDescriptor
                {
                    private:
                        CoreOS::common::uint16_t LimitLow;
                        CoreOS::common::uint16_t BaseLow;
                        CoreOS::common::uint8_t BaseHigh;
                        CoreOS::common::uint8_t uType;
                        CoreOS::common::uint8_t FlagsLimitHigh;
                        CoreOS::common::uint8_t BaseVHigh;

                    public:
                        SegmentDescriptor( CoreOS::common::uint32_t uBase, CoreOS::common::uint32_t uLimit, CoreOS::common::uint8_t Type );
                        CoreOS::common::uint32_t Base( );
                        CoreOS::common::uint32_t Limit( );

                } __attribute__( ( packed ) );

            private:
                SegmentDescriptor nullSegmentSelector;
                SegmentDescriptor unusedSegmentSelector;
                SegmentDescriptor codeSegmentSelector;
                SegmentDescriptor dataSegmentSelector;

            public:
                GlobalDescriptorTable( );
                ~GlobalDescriptorTable( );

                CoreOS::common::uint16_t CodeSegmentSelector( );
                CoreOS::common::uint16_t DataSegmentSelector( );
        };
    }
}

#endif