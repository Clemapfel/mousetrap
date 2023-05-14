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

                add_page("Point", Shape::Point({0, 0}));
                add_page("Points", Shape::Points({{-0.5, 0.5}, {+0.5, 0.5}, {0, -0.5}}));
                add_page("Triangle", Shape::Triangle({-0.5, 0.5}, {+0.5, 0.5}, {0, -0.5}));
                add_page("Rectangle", Shape::Rectangle({-0.5, 0.5}, {0.5, 0.5}, {0.5, -0.5}, {-0.5, -0.5}));
                add_page("Circle", Shape::Circle({0, 0}, 0.5, 32));
                add_page("Ellipse", Shape::Ellipse({0, 0}, 0.75, 0.5, 32));
                add_page("Line", Shape::Line({-0.5, 0.5}, {0.5, -0.5}));
                add_page("Lines", Shape::Lines({{{-0.5, 0.5}, {0.5, -0.5}}, {{0.5, 0.5}, {-0.5, -0.5}}}));
                add_page("Line Strip", Shape::LineStrip({{-0.5, 0.5}, {0.5, -0.5}, {0.5, 0.5}, {-0.5, -0.5}}));
                add_page("Polygon", Shape::Polygon({{0, 0.75}, {0.75, 0.25}, {0.5, -0.75}, {-0.5, -0.5}, {-0.75, 0}, {0, 0.75}}));
                add_page("Rectangle Frame", Shape::RectangleFrame({-0.5, 0.5}, {1, 1}, 0.15, 0.15));

                key_controller.connect_signal_key_pressed([](KeyEventController*, int32_t key, int32_t, ModifierState, ShapeShowcase* instance){

                    auto current = instance->stack.get_selection_model()->get_selection().at(0);
                    if (key == GDK_KEY_Left)
                    {
                        auto* model = instance->stack.get_selection_model();
                        size_t current = model->get_selection().at(0);
                        size_t max = instance->stack.get_n_children();

                        if (current > 0)
                            model->select(current-1);
                        else
                            model->select(max-1);
                    }
                    else if (key == GDK_KEY_Right)
                    {
                        auto* model = instance->stack.get_selection_model();
                        size_t current = model->get_selection().at(0);
                        size_t max = instance->stack.get_n_children();

                        if (current+1 < max)
                            model->select(current+1);
                        else
                            model->select(0);
                    }

                    return true;
                }, this);

                stack.add_controller(key_controller);
                main.push_back(stack);
            }

            operator NativeWidget() const override
            {
                return main.operator NativeWidget();
            }
    };
}
