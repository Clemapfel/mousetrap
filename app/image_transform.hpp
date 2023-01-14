//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 1/14/23
//

#pragma once

#include <app/global_state.hpp>

namespace mousetrap::state::actions
{
    Action color_transform_invert_action = Action("color_transform.invert");
    Action color_transform_to_grayscale_action = Action("color_transform.to_grayscale");
    Action color_transform_open_hsv_shift_dialog_action = Action("color_transform.open_hsv_shift_dialog");
    Action color_transform_open_color_replace_dialog_action = Action("color_tranform.open_color_replace_dialog");
    Action color_transform_open_palette_shift_dialog_action = Action("color_transform.open_palette_shift_dialog");

    Action image_transform_open_scale_canvas_dialog_action = Action("image_transform.open_scale_canvas_dialog");
    Action image_transform_open_resize_canvas_dialog_action = Action("image_transform.open_resize_canvas_dialog");

    Action image_transform_flip_horizontally_action = Action("image_transform.flip_horizontally");
    Action image_transform_flip_vertically_action = Action("image_tranform.flip_vertically");
    Action image_transform_rotate_clockwise_action = Action("image_transform.rotate_clockwise");
    Action image_transform_rotate_counterclockwise_action = Action("image_transform.rotate_counterclockwise");
}
