//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 1/14/23
//

#pragma once

#include <app/project_state.hpp>

namespace mousetrap
{
    namespace state::actions
    {
        DECLARE_GLOBAL_ACTION(image_transform, open_scale_canvas_dialog);
        DECLARE_GLOBAL_ACTION(image_transform, open_resize_canvas_dialog);

        DECLARE_GLOBAL_ACTION(image_transform, flip_horizontally);
        DECLARE_GLOBAL_ACTION(image_transform, flip_vertically);
        DECLARE_GLOBAL_ACTION(image_transform, rotate_clockwise);
        DECLARE_GLOBAL_ACTION(image_transform, rotate_counterclockwise);
    }

    class ImageTransform
    {
        public:
            ImageTransform();

            void rotate_clockwise();
            void rotate_counterclockwise();
            void flip_horizontally();
            void flip_vertically();

        private:
    };

    namespace state
    {
        inline ImageTransform* image_transform = nullptr;
    }
}
