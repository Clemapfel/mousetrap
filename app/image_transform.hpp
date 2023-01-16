//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 1/14/23
//

#pragma once

#include <app/global_state.hpp>

namespace mousetrap
{
    namespace state::actions
    {
        Action image_transform_open_scale_canvas_dialog_action = Action("image_transform.open_scale_canvas_dialog");
        Action image_transform_open_resize_canvas_dialog_action = Action("image_transform.open_resize_canvas_dialog");

        Action image_transform_flip_horizontally_action = Action("image_transform.flip_horizontally");
        Action image_transform_flip_vertically_action = Action("image_tranform.flip_vertically");
        Action image_transform_rotate_clockwise_action = Action("image_transform.rotate_clockwise");
        Action image_transform_rotate_counterclockwise_action = Action("image_transform.rotate_counterclockwise");
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
        ImageTransform* image_transform = nullptr;
    }
}
