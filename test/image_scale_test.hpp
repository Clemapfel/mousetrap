//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 4/28/23
//

#pragma once

#include <mousetrap.hpp>

namespace mousetrap
{
    class ImageScaleTest : public Widget
    {
        private:
            Image original_data;

            ImageDisplay scale_2;
            Label scale_2_label = Label("2x");
            Box scale_2_box = Box(Orientation::VERTICAL);

            ImageDisplay scale_15;
            Label scale_15_label = Label("1.5x");
            Box scale_15_box = Box(Orientation::VERTICAL);

            ImageDisplay scale_1;
            Label scale_1_label = Label("1x");
            Box scale_1_box = Box(Orientation::VERTICAL);

            ImageDisplay scale_075;
            Label scale_075_label = Label("0.75x");
            Box scale_075_box = Box(Orientation::VERTICAL);

            ImageDisplay scale_05;
            Label scale_05_label = Label("0.5x");
            Box scale_05_box = Box(Orientation::VERTICAL);

            ListView display_box = ListView(Orientation::HORIZONTAL);

            DropDown dropdown;

            Box window_box = Box(Orientation::VERTICAL);
            ScrolledWindow window;

        public:
            ImageScaleTest()
            {
                const Vector2ui size = {10, 10};

                original_data.create(size.x, size.y, RGBA(1, 1, 1, 1));

                for (size_t x = 0; x < size.x; ++x)
                    for (size_t y = 0; y < size.y; ++y)
                        if (x % 2 == 0 xor y % 2 == 1)
                            original_data.set_pixel(x, y, HSVA(glm::distance(Vector2f(x, y), Vector2f(0.5 * size.x - 0.5, 0.5 * size.y - 0.5)) / (size.x * 0.5) , 1, 1, 1));


                scale_2_box.push_back(scale_2);
                scale_2_box.push_back(scale_2_label);

                scale_15_box.push_back(scale_15);
                scale_15_box.push_back(scale_15_label);

                scale_1_box.push_back(scale_1);
                scale_1_box.push_back(scale_1_label);

                scale_075_box.push_back(scale_075);
                scale_075_box.push_back(scale_075_label);

                scale_05_box.push_back(scale_05);
                scale_05_box.push_back(scale_05_label);

                for (auto* box : {
                    &scale_2_box,
                    &scale_15_box,
                    &scale_1_box,
                    &scale_075_box,
                    &scale_05_box,
                })
                    display_box.push_back(*box);

                for (auto* display : {
                    &scale_2,
                    &scale_15,
                    &scale_1,
                    &scale_075,
                    &scale_05,
                }) {
                    display->set_expand(false);
                    display->set_alignment(Alignment::CENTER);
                    display->set_margin(10);
                }

                for (auto pair : std::vector<std::pair<std::string, InterpolationType>>{
                    {"NEAREST", InterpolationType::NEAREST},
                    {"BILINEAR", InterpolationType::BILINEAR},
                    {"HYPERBOLIC", InterpolationType::HYPERBOLIC},
                    {"TILES", InterpolationType::TILES}
                })
                {
                    dropdown.push_back( Label(pair.first), Label("InterpolationType::<b>" + pair.first + "</b>"),[type = pair.second](DropDown*, ImageScaleTest* self)
                    {
                        auto new_size = Vector2ui(150, 150);

                        auto scaled = [&](float scale){
                            return self->original_data
                                .as_scaled(scale * self->original_data.get_size().x, scale * self->original_data.get_size().y, type)
                                .as_scaled(new_size.x, new_size.y, InterpolationType::NEAREST);
                        };

                        self->scale_2.create_from_image(scaled(2));
                        self->scale_15.create_from_image(scaled(1.5));
                        self->scale_1.create_from_image(scaled(1));
                        self->scale_075.create_from_image(scaled(0.75));
                        self->scale_05.create_from_image(scaled(0.5));

                        for (auto* display : {
                            &self->scale_2,
                            &self->scale_15,
                            &self->scale_1,
                            &self->scale_075,
                            &self->scale_05,
                        })
                            display->set_size_request(new_size);

                    }, this);
                }
                dropdown.set_margin(10);

                window_box.push_back(display_box);
                window_box.push_back(Separator());
                window_box.push_back(dropdown);

                window.set_child(window_box);
            }

            operator NativeWidget() const override
            {
                return window.operator NativeWidget();
            }
    };
}
