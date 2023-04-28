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


            ImageDisplay original;
            Label original_label = Label("1x");
            Box original_box = Box(Orientation::VERTICAL);

            ImageDisplay scaled_up;
            Label scaled_up_label = Label("1.5x");
            Box scaled_up_box = Box(Orientation::VERTICAL);

            ImageDisplay scaled_down;
            Label scaled_down_label = Label("0.5x");
            Box scaled_down_box = Box(Orientation::VERTICAL);

            CenterBox display_box = CenterBox(Orientation::HORIZONTAL);

            DropDown dropdown;

            Box main = Box(Orientation::VERTICAL);

        public:
            ImageScaleTest()
            {
                const Vector2ui size = {10, 10};

                original_data.create(size.x, size.y, RGBA(0, 0, 0, 1));

                for (size_t x = 0; x < size.x; ++x)
                    for (size_t y = 0; y < size.y; ++y)
                        if (x % 2 == 0 xor y % 2 == 1)
                            original_data.set_pixel(x, y, RGBA(1, 1, 1, 1));

                original_data = original_data.as_scaled(50, 50, InterpolationType::NEAREST);
                original.create_from_image(original_data);
                original.set_size_request(original.get_size());

                original_box.push_back(original);
                original_box.push_back(original_label);

                scaled_up_box.push_back(scaled_up);
                scaled_up_box.push_back(scaled_up_label);

                scaled_down_box.push_back(scaled_down);
                scaled_down_box.push_back(scaled_down_label);

                display_box.set_center_widget(original_box);
                display_box.set_start_widget(scaled_up_box);
                display_box.set_end_widget(scaled_down_box);
                display_box.set_margin(50);

                for (auto* display : {
                    &original,
                    &scaled_up,
                    &scaled_down
                }) {
                    display->set_expand(false);
                    display->set_alignment(Alignment::CENTER);
                }

                for (auto pair : std::vector<std::pair<std::string, InterpolationType>>{
                    {"NEAREST", InterpolationType::NEAREST},
                    {"BILINEAR", InterpolationType::BILINEAR},
                    {"HYPERBOLIC", InterpolationType::HYPERBOLIC},
                    {"TILES", InterpolationType::TILES}
                })
                {
                    dropdown.push_back(Label(pair.first), Label(pair.first), [type = pair.second](DropDown*, ImageScaleTest* self)
                    {
                        auto size = self->original_data.get_size();
                        auto scaled_up_data = self->original_data.as_scaled(1.5 * size.x, 1.5 * size.y, type);
                        self->scaled_up.create_from_image(scaled_up_data);
                        self->scaled_up.set_size_request(self->scaled_up.get_size());

                        auto scaled_down_data = self->original_data.as_scaled(0.5 * size.x, 0.5 * size.y, type);
                        self->scaled_down.create_from_image(scaled_down_data);
                        self->scaled_down.set_size_request(self->scaled_down.get_size());

                    }, this);
                }
                dropdown.set_margin(10);

                main.push_back(display_box);
                main.push_back(Separator());
                main.push_back(dropdown);
            }

            operator NativeWidget() const override
            {
                return main.operator NativeWidget();
            }
    };
}
