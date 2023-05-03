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
            RenderArea _area;
            Frame _main;

            Shape _shape;

        public:
            RenderAreaTest()
            {
                _area.connect_signal_realize([](Widget* widget, RenderAreaTest* instance){
                    auto* area = (RenderArea*) widget;
                    area->make_current();

                    instance->_shape = Shape();
                    instance->_shape.as_rectangle({0, 0}, {1, 1});
                    area->add_render_task(&instance->_shape);

                }, this);
                _main.set_child(_area);
            }

            operator NativeWidget() const override
            {
                return _main.operator NativeWidget();
            }
    };
}
