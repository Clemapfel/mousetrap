//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 1/14/23
//

#include <app/selection.hpp>

namespace mousetrap
{
    Selection::Selection()
    {}

    void Selection::create_from(const Vector2iSet& set)
    {
        int x_min = std::numeric_limits<int>::max();
        int x_max = std::numeric_limits<int>::min();
        int y_min = std::numeric_limits<int>::max();
        int y_max = std::numeric_limits<int>::min();

        for (const auto& vec : set)
        {
            if (vec.x <= x_min)
                x_min = vec.x;

            if (vec.x >= x_max)
                x_max = vec.x;

            if (vec.y <= y_min)
                y_min = vec.y;

            if (vec.y >= y_max)
                y_max = vec.y;
        }

        _data.clear();
        for (int y = y_min; y <= y_max; ++y)
        {
            std::vector<std::pair<int, int>>* vec;
            if (_data.find(y) == _data.end())
                vec = &_data.insert({y, {}}).first->second;

            vec->push_back({x_min, x_min});
            for (int x = x_min; x <= x_max; ++x)
            {
                auto it = set.find({x, y});
                if (it == set.end())
                {
                    if (vec->back().second == x)
                        vec->push_back({});
                }
                else
                {
                }

            }
        }

        for (auto& pair : _data)
        {
            std::cout << "x: " << pair.first << " -> {";
            for (auto& range : pair.second)
            {
                std::cout << "[" << range.first << " | " << range.second << "], ";
            }
            std::cout << "}\n";
        }
    }
}