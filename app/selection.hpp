//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 1/14/23
//

#pragma once

#include <mousetrap.hpp>
#include <app/app_signals.hpp>

namespace mousetrap
{
    class Selection
    {
        public:
            Selection();

            /// @brief create from set of points
            void create_from(const Vector2iSet&);
            void create_from_rectangle(Vector2i top_left, Vector2i size);

            bool at(Vector2i) const;
            void apply_offset(Vector2i);
            void invert(int x_min, int y_min, int x_max, int y_max);

            void subtract(const Selection&);
            void add(const Selection&);

            using OutlineVertices = struct
            {
                std::vector<std::pair<Vector2f, Vector2f>> left_to_right;
                std::vector<std::pair<Vector2f, Vector2f>> right_to_left;
                std::vector<std::pair<Vector2f, Vector2f>> top_to_bottom;
                std::vector<std::pair<Vector2f, Vector2f>> bottom_to_top;
            };
            const OutlineVertices& get_outline_vertices() const;

        private:
            void generate_outline_vertices();
            OutlineVertices _outline_vertices;

            // y -> {(x_min, x_max), ...}
            Vector2iSet _data;
            int _x_min = 0;
            int _x_max = 0;
            int _y_min = 0;
            int _y_max = 0;
    };

}
