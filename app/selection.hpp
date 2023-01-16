//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 1/14/23
//

#pragma once

#include <app/global_state.hpp>

namespace mousetrap
{
    namespace state::actions
    {
        inline Action selection_select_all_action = Action("selection.select_all");
        inline Action selection_invert_action = Action("selection.invert");
        inline Action selection_open_select_color_dialog_action = Action("selection.open_select_color_dialog");
    }
}
