#ifndef __COREOS__GUI__WIDGET_H
#define __COREOS__GUI__WIDGET_H

#include <common/types.h>

namespace CoreOS
{
    namespace gui
    {
        class Widget
        {
            protected:
                Widget* parent;
                common::int32_t x;
                common::int32_t y;
                common::int32_t w;
                common::int32_t h;

                common::uint8_t r;
                common::uint8_t g;
                common::uint8_t b;

            public:
                virtual void Draw( GraphicsContext* gc );
                virtual void OnMouseDown( common::int32_t x, common::int32_t y );
                virtual void OnMouseUp( common::int32_t x, com
        };
    }
}

#endif