//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 4/30/23
//

#pragma once

#include <mousetrap.hpp>

namespace mousetrap
{
    class ShapeShowcase : public Widget
    {
        private:

            Stack stack;
            Box main = Box(Orientation::HORIZONTAL);

            KeyEventController key_controller;

        public:
            ShapeShowcase()
            {
                auto add_page = [&](const std::string& string, const Shape& shape){

                    auto area = RenderArea();
                    area.add_render_task(shape);

                    auto box = Box(Orientation::VERTICAL);

                    auto overlay = Overlay();
                    overlay.set_child(Separator());
                    overlay.add_overlay(area, true);

                    auto frame = Frame();
                    frame.set_child(overlay);

                    auto aspect_frame = AspectFrame(1);
                    aspect_frame.set_child(frame);
                    aspect_frame.set_margin(10);

                    auto label = Label(string);
                    box.push_back(aspect_frame);
                    box.push_back(label);
                    box.set_margin(10);

                    area.set_size_request({200, 200});

                    stack.add_child(box, string);
                };

                add_page("Point", Point({0, 0}));

                main.add_controller(key_controller);
                main.push_back(stack);
            }

            operator NativeWidget() const override
            {
                return main.operator NativeWidget();
            }
    };
}
