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

            Texture* _texture;

            Box _box = Box(Orientation::HORIZONTAL);

        public:
            RenderAreaTest()
            {

                auto image = Image();
                image.create(10, 10, RGBA(0, 0, 0, 1));
                for (size_t x = 0; x < image.get_size().x; ++x)
                    for (size_t y = 0; y < image.get_size().y; ++y)
                        if (x % 2 == 0 and y % 2 != 0)
                            image.set_pixel(x, y, RGBA(1, 1, 1, 1));

                _texture = new Texture();
                _texture->create_from_image(image);

                _left.connect_signal_realize([](Widget* widget, RenderAreaTest* instance)
                {
                    auto* area = (RenderArea*) widget;

                    instance->_left_shape.as_rectangle({-1, -1}, {2, 2});
                    instance->_left_shape.set_color(RGBA(1, 0, 1, 1));
                    instance->_left_shape.set_texture(instance->_texture);
                    area->add_render_task(instance->_left_shape);

                }, this);

                _right.connect_signal_realize([](Widget* widget, RenderAreaTest* instance)
                {
                    auto* area = (RenderArea*) widget;

                    instance->_right_shape.as_rectangle({-1, -1}, {2, 2});
                    instance->_right_shape.set_color(RGBA(0, 1, 0, 1));
                    instance->_right_shape.set_texture(instance->_texture);
                    area->add_render_task(instance->_right_shape);

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
