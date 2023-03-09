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
        for (int x = x_min; x <= x_max; ++x)
        {
            for (int y = y_min; y <= y_max; ++y)
            {
                auto it = set.find({x, y});
                if (it == set.end())
                    continue;
            }
        }

        for (auto& pair : _data)
            std::cout << "x: " << pair.first << " -> [" << pair.second.first << " | " << pair.second.second << "]" << std::endl;
    }
}