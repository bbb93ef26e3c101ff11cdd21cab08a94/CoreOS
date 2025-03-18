#ifndef __COREOS__HARDWARECOMMUNICATION__PORT_H
#define __COREOS__HARDWARECOMMUNICATION__PORT_H

#include <common/types.h>

namespace CoreOS
{
    namespace hardwarecommunication
    {
        class Port
        {
            protected:
                Port(CoreOS::common::uint16_t PortNumber);
                ~Port();
                CoreOS::common::uint16_t PortNumber;
        };

        class Port8Bit : public Port
        {
            public:
                Port8Bit(CoreOS::common::uint16_t PortNumber);
                ~Port8Bit();

                virtual CoreOS::common::uint8_t Read();
                virtual void Write(CoreOS::common::uint8_t Data);
        };

        class Port8BitSlow : public Port8Bit
        {
            public:
                Port8BitSlow(CoreOS::common::uint16_t PortNumber);
                ~Port8BitSlow();
                virtual void Write(CoreOS::common::uint8_t Data);
        };

        class Port16Bit : public Port
        {
            public:
                Port16Bit(CoreOS::common::uint16_t PortNumber);
                ~Port16Bit();
                virtual CoreOS::common::uint16_t Read();
                virtual void Write(CoreOS::common::uint16_t Data);
        };

        class Port32Bit : public Port
        {
            public:
                Port32Bit(CoreOS::common::uint16_t PortNumber);
                ~Port32Bit();

                virtual CoreOS::common::uint32_t Read();
                virtual void Write(CoreOS::common::uint32_t Data);
        };
    }
}

#endif