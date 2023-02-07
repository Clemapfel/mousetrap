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

    inline std::string apply_scope_to_string(ApplyScope scope)
    {
        if (scope == ApplyScope::CURRENT_CELL)
            return "Current Cell";
        else if (scope == ApplyScope::CURRENT_LAYER)
            return "Current Layer, All Frames";
        else if (scope == ApplyScope::CURRENT_FRAME)
            return "Current Frame, All Layers";
        else if (scope == ApplyScope::EVERYWHERE)
            return "All Layers and Frames";
    }
}
