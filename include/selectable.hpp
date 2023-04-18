//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 3/25/23
//

#pragma once

#include <include/selection_model.hpp>

namespace mousetrap
{
    /// @brief a container widgets whose items are selectable
    class Selectable
    {
        public:
            /// @brief expose selection model
            /// @return selection model
            virtual SelectionModel* get_selection_model() = 0;
    };
}
