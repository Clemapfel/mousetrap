//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 2/7/23
//

#pragma once

#include <string>

namespace mousetrap
{
    enum ApplyScope
    {
        CURRENT_CELL,
        CURRENT_FRAME,
        CURRENT_LAYER,
        EVERYWHERE
    };

    std::string apply_scope_to_string(ApplyScope);
}
