#ifndef __COREOS__DRIVERS__VIDEO__VGA_H
#define __COREOS__DRIVERS__VIDEO__VGA_H

#include <common/types.h>
#include <hardwarecommunication/port.h>
#include <drivers/driver.h>

namespace CoreOS
{
    namespace drivers
    {
        namespace video
        {
            class VideoGraphicsArray
            {
                protected:
                    hardwarecommunication::Port8Bit MiscPort;
                    hardwarecommunication::Port8Bit crtcIndexPort;
                    hardwarecommunication::Port8Bit crtcDataPort;
                    hardwarecommunication::Port8Bit SequencerIndexPort;
                    hardwarecommunication::Port8Bit SequencerDataPort;
                    hardwarecommunication::Port8Bit GraphicsControllerIndexPort;
                    hardwarecommunication::Port8Bit GraphicsControllerDataPort;
                    hardwarecommunication::Port8Bit AttributeControllerIndexPort;
                    hardwarecommunication::Port8Bit AttributeControllerReadPort;
                    hardwarecommunication::Port8Bit AttributeControllerWritePort;
                    hardwarecommunication::Port8Bit AttributeControllerResetPort;

                    void WriteRegisters( common::uint8_t* uRegisters );
                    common::uint8_t* GetFrameBufferSegment( ); 

                    virtual common::uint8_t GetColorIndex( common::uint8_t r, common::uint8_t g, common::uint8_t b );

                public:
                    VideoGraphicsArray( );
                    ~VideoGraphicsArray( );

                    virtual bool SupportsMode( common::uint32_t nWidth, common::uint32_t nHeight, common::uint32_t ColorDepth );
                    virtual bool SetMode( common::uint32_t nWidth, common::uint32_t nHeight, common::uint32_t ColorDepth );
                    virtual void PutPixel( common::uint32_t x, common::uint32_t y, common::uint8_t r, common::uint8_t g, common::uint8_t b );
                    virtual void PutPixel( common::uint32_t x, common::uint32_t y, common::uint8_t ColorIndex );
            };
        }
    }
}

#endif