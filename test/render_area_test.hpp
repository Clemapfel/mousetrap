//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 4/28/23
//

#pragma once

#include <mousetrap.hpp>

namespace mousetrap
{
    class RenderAreaTest : public Widget
    {
        private:
            RenderArea _left;
            Shape _left_shape;

            RenderArea _right;
            Shape _right_shape;

            Box _box = Box(Orientation::HORIZONTAL);

        public:
            RenderAreaTest()
            {
                _left.connect_signal_realize([](Widget* widget, RenderAreaTest* instance)
                {
                    auto* area = (RenderArea*) widget;

                    instance->_left_shape.as_rectangle({-1, -1}, {2, 2});
                    instance->_left_shape.set_color(RGBA(1, 0, 1, 1));
                    area->add_render_task(&instance->_left_shape);

                }, this);

                _right.connect_signal_realize([](Widget* widget, RenderAreaTest* instance)
                {
                    auto* area = (RenderArea*) widget;

                    instance->_right_shape.as_rectangle({-1, -1}, {2, 2});
                    instance->_right_shape.set_color(RGBA(0, 1, 0, 1));
                    area->add_render_task(&instance->_right_shape);

                }, this);

                _left.set_expand(true);
                _right.set_expand(true);

                _box.push_back(_left);
                _box.push_back(_right);
            }

            operator NativeWidget() const override
            {
                return _box.operator NativeWidget();
            }
    };
}
