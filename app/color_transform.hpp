//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 1/16/23
//

#pragma once

#include <mousetrap.hpp>
#include <app/add_shortcut_action.hpp>

namespace mousetrap
{
    namespace state::actions
    {
        Action color_transform_invert_action = Action("color_transform.invert");
        Action color_transform_to_grayscale_action = Action("color_transform.to_grayscale");
        Action color_transform_open_hsv_shift_dialog_action = Action("color_transform.open_hsv_shift_dialog");
        Action color_transform_open_color_replace_dialog_action = Action("color_tranform.open_color_replace_dialog");
        Action color_transform_open_palette_shift_dialog_action = Action("color_transform.open_palette_shift_dialog");
    }

    class ColorTransform
    {
        public:
            ColorTransform();

            void invert();
            void to_grayscale();
            void hsv_shift(float h_offset, float s_offset, float v_offset);

        private:
            void restore_from_grayscale_backup();
            std::vector<std::vector<float>> grayscale_backup;

            void restore_from_hsv_shift_backup();
    };

    namespace state
    {
        ColorTransform* color_transform = nullptr;
    }
}

// ###

/*
namespace mousetrap
{
    ColorTransform::ColorTransform()
    {
        using namespace state::actions;

        // invert

        auto color_transform_invert_do = [](){
            state::color_transform->invert();
        };
        color_transform_invert_action.set_function(color_transform_invert_do, color_transform_invert_do, color_transform_invert_do);
        state::add_shortcut_action(color_transform_invert_action);

        // to grayscale

        auto color_transform_to_grayscale_do = [](){
            state::color_transform->to_grayscale();
        };
        auto color_transform_to_grayscale_undo = [](){
            state::color_transform->restore_from_grayscale_backup();
        };
        color_transform_to_grayscale_action.set_function(color_transform_to_grayscale_do, color_transform_to_grayscale_undo, color_transform_to_grayscale_do);
        state::add_shortcut_action(color_transform_to_grayscale_action);

    }
}
 */