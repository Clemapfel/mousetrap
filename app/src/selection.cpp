//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 1/14/23
//

#include <app/selection.hpp>
#include <app/algorithms.hpp>

namespace mousetrap
{
    Selection::Selection()
    {}

    bool Selection::at(Vector2i xy) const
    {
         auto it = _data.find(xy.y);
         if (it == _data.end())
             return false;

         for (auto& pair : it->second)
             if (pair.first <= xy.x and pair.second >= xy.x)
                 return true;

         return false;
    }

    void Selection::create_from(const Vector2iSet& set)
    {
        _x_min = std::numeric_limits<int>::max();
        _x_max = std::numeric_limits<int>::min();
        _y_min = std::numeric_limits<int>::max();
        _y_max = std::numeric_limits<int>::min();

        for (const auto& vec : set)
        {
            if (vec.x <= _x_min)
                _x_min = vec.x;

            if (vec.x >= _x_max)
                _x_max = vec.x;

            if (vec.y <= _y_min)
                _y_min = vec.y;

            if (vec.y >= _y_max)
                _y_max = vec.y;
        }

        _data.clear();
        for (int y = _y_min; y <= _y_max; ++y)
        {
            std::vector<std::pair<int, int>> vec = {};
            for (int x = _x_min; x <= _x_max; ++x)
            {
                auto it = set.find({x, y});
                if (it == set.end())
                    continue;

                if (vec.empty())
                    vec.push_back({x, x});
                else if (x == vec.back().second + 1)
                    vec.back().second += 1;
                else
                    vec.push_back({x, x});
            }

            if (not vec.empty())
                _data[y] = vec;
        }

        for (int y = _y_min; y < _y_max; ++y)
        {
            auto it = _data.find(y);
            if (it == _data.end())
                continue;

            for (auto& range : it->second)
            {
                int x1 = range.first;
                int x2 = range.second;
                _outline_vertices.bottom_to_top.push_back({{x1, y}, {x1, y+1}});
                _outline_vertices.top_to_bottom.push_back({{x2, y}, {x2, y-1}});
            }
        }

        std::map<int, std::vector<std::pair<int, int>>> data_by_x;
        for (int x = _x_min; x <= _x_max; ++x)
        {
            std::vector<std::pair<int, int>> vec = {};
            for (int y = _y_min; y <= _y_max; ++y)
            {
                auto it = set.find({x, y});
                if (it == set.end())
                    continue;

                if (vec.empty())
                    vec.push_back({y, y});
                else if (y == vec.back().second + 1)
                    vec.back().second += 1;
                else
                    vec.push_back({y, y});
            }

            if (not vec.empty())
                data_by_x[x] = vec;
        }

        for (int x = _x_min; x < _x_max; ++x)
        {
            auto it = data_by_x.find(x);
            if (it == data_by_x.end())
                continue;

            for (auto& range : it->second)
            {
                int y1 = range.first;
                int y2 = range.second;
                _outline_vertices.left_to_right.push_back({{x, y1}, {x+1, y1}});
                _outline_vertices.right_to_left.push_back({{x, y2}, {x-1, y2}});
            }
        }
    }

    void Selection::create_from_rectangle(Vector2i top_left, Vector2i size)
    {
        _data.clear();
        for (int y = top_left.y; y < top_left.y + size.y; ++y)
            _data.insert({y, {{top_left.x, top_left.x + size.x}}});

        _outline_vertices.left_to_right = {
            {{top_left.x, top_left.y}, {top_left.x + size.x, top_left.y}}
        };

        _outline_vertices.right_to_left = {
            {{top_left.x + size.x, top_left.y + size.y}, {top_left.x, top_left.y + size.y}}
        };

        _outline_vertices.bottom_to_top = {
            {{top_left.x, top_left.y}, {top_left.x, top_left.y + size.y}}
        };

        _outline_vertices.top_to_bottom = {
            {{top_left.x + size.x, top_left.y}, {top_left.x + size.x, top_left.y + size.y}}
        };

        _x_min = top_left.x;
        _x_max = top_left.x + size.x;
        _y_min = top_left.y;
        _y_max = top_left.y + size.y;
    }

    void Selection::apply_offset(Vector2i offset)
    {
        std::map<int, std::vector<std::pair<int, int>>> new_data;
        for (auto& pair : _data)
        {
            auto to_push = std::pair<int, std::vector<std::pair<int, int>>>();
            to_push.first = pair.first + offset.y;

            for (auto& range : pair.second)
                to_push.second.push_back({range.first + offset.x, range.second + offset.x});

            new_data.insert(to_push);
        }

        _data = std::move(new_data);
    }

    Selection::operator std::string()
    {
        std::stringstream out;
        for (auto& pair : _data)
        {
            out << pair.first << ": {";
            for (auto& range : pair.second)
                out << "[" << range.first << " | " << range.second << "] ";
            out << "}\n";
        }
        return out.str();
    }

    const Selection::OutlineVertices& Selection::get_outline_vertices() const
    {
        return _outline_vertices;
    }
}