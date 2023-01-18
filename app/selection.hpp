//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 1/14/23
//

#pragma once

#include <app/global_state.hpp>

namespace mousetrap
{
    namespace state::actions
    {
        DECLARE_GLOBAL_ACTION(selection, select_all);
        DECLARE_GLOBAL_ACTION(selection, invert);
        DECLARE_GLOBAL_ACTION(selection, open_select_color_dialog);
    }
}
