//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 1/14/23
//

#pragma once

#include <mousetrap.hpp>
#include <app/app_signals.hpp>

namespace mousetrap
{
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

            bool at(Vector2i) const;
            void apply_offset(Vector2i);
            void invert(int x_min, int y_min, int x_max, int y_max);

            void subtract(const Selection&);
            void add(const Selection&);

            operator std::string() const;
            explicit operator Vector2iSet() const;

            using OutlineVertices = struct
            {
                std::vector<std::pair<Vector2f, Vector2f>> left_to_right;
                std::vector<std::pair<Vector2f, Vector2f>> right_to_left;
                std::vector<std::pair<Vector2f, Vector2f>> top_to_bottom;
                std::vector<std::pair<Vector2f, Vector2f>> bottom_to_top;
            };
            const OutlineVertices& get_outline_vertices() const;

            //auto begin();
            //auto end();

        private:
            void generate_outline_vertices();
            OutlineVertices _outline_vertices;

            // y -> {(x_min, x_max), ...}
            using Data_t = std::map<int, std::vector<std::pair<int, int>>>;
            Data_t _data;

            int _x_min = 0;
            int _x_max = 0;
            int _y_min = 0;
            int _y_max = 0;

            /*
            struct Iterator
            {
                public:
                    Iterator(Selection*, Data_t::iterator);
                    operator Vector2i();

                    Iterator& operator ++();
                    Iterator& operator ++(int);
                    bool operator==(const Iterator& other);


                private:
                    Selection* _owner;
                    Data_t::iterator _it;
                    size_t current_i = 0;
            };
             */
    };

}
