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
        DECLARE_GLOBAL_ACTION(color_transform, invert);
        DECLARE_GLOBAL_ACTION(color_transform, to_grayscale);
        DECLARE_GLOBAL_ACTION(color_transform, open_hsv_shift_dialog);
        DECLARE_GLOBAL_ACTION(color_transform, open_color_replace_dialog);
        DECLARE_GLOBAL_ACTION(color_transform, open_palette_shift_dialog);
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
        inline ColorTransform* color_transform = nullptr;
    }
}
