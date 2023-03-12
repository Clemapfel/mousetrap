//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 1/14/23
//

#pragma once

#include <app/app_signals.hpp>

namespace mousetrap
{
    namespace state::actions
    {
        DECLARE_GLOBAL_ACTION(selection, select_all);
        DECLARE_GLOBAL_ACTION(selection, invert);
        DECLARE_GLOBAL_ACTION(selection, open_select_color_dialog);
    }

    enum class SelectionMode
    {
        REPLACE,
        ADD,
        SUBTRACT
    };

    class Selection
    {
        public:
            Selection();

            /// @brief create from set of points
            void create_from(const Vector2iSet&);
            void create_from_rectangle(Vector2i top_left, Vector2i size);

            bool at(Vector2i);
            void apply_offset(Vector2i);

            operator std::string();

        private:
            // y -> {(x_min, x_max), ...}
            std::map<int, std::vector<std::pair<int, int>>> _data;
    };

}
