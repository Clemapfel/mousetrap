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

    class Selection
    {
        public:
            Selection();

            void create_from(const Vector2iSet&);
            bool at(Vector2i);

        private:
            // x coords -> (y_min, y_max)
            std::map<int, std::pair<int, int>> _data;
    };

}
