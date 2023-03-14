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
         return _data.find(xy) != _data.end();
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

        _data = set;
        generate_outline_vertices();
    }

    void Selection::create_from_rectangle(Vector2i top_left, Vector2i size)
    {
        _data.clear();

        _x_min = top_left.x;
        _y_min = top_left.y;
        _x_max = top_left.x + size.x;
        _y_max = top_left.y + size.y;

        for (int x = _x_min; x <= _x_max; ++x)
            for (int y = _y_min; y <= _y_max; ++y)
                _data.insert({x, y});

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
    }

    void Selection::apply_offset(Vector2i offset)
    {
        auto new_set = Vector2iSet();
        for (auto pair : _data)
            new_set.insert(pair + offset);

        create_from(new_set);
    }

    const Selection::OutlineVertices& Selection::get_outline_vertices() const
    {
        return _outline_vertices;
    }

    void Selection::invert(int x_min, int y_min, int x_max, int y_max)
    {
         auto set = Vector2iSet();
         for (int x = x_min; x < x_max; ++x)
             for (int y = y_min; y < y_max; ++y)
                 if (not at({x, y}))
                     set.insert({x, y});

        create_from(set);
    }

    void Selection::add(const Vector2iSet& other)
    {
        auto before = _data.size();
        auto add = other.size();

        for (auto pos : other)
        {
            if (pos.x < _x_min)
                _x_min = pos.x;

            if (pos.y < _y_min)
                _y_min = pos.y;

            if (pos.x > _x_max)
                _x_max = pos.x;

            if (pos.y > _y_max)
                _y_max = pos.y;

            _data.insert(pos);
        }

        generate_outline_vertices();
    }

    void Selection::subtract(const Vector2iSet& other)
    {
        for (auto pos : other)
            _data.erase(pos);

        for (auto& pos : _data)
        {
            if (pos.x < _x_min)
                _x_min = pos.x;

            if (pos.y < _y_min)
                _y_min = pos.y;

            if (pos.x > _x_max)
                _x_max = pos.x;

            if (pos.y > _y_max)
                _y_max = pos.y;
        }

        generate_outline_vertices();
    }

    void Selection::generate_outline_vertices()
    {
        _outline_vertices = OutlineVertices();

        std::map<int, std::vector<std::pair<int, int>>> data_by_y;
        for (int y = _y_min; y <= _y_max; ++y)
        {
            std::vector<std::pair<int, int>> vec = {};
            for (int x = _x_min; x <= _x_max; ++x)
            {
                auto it = _data.find({x, y});
                if (it == _data.end())
                    continue;

                if (vec.empty())
                    vec.push_back({x, x});
                else if (x == vec.back().second + 1)
                    vec.back().second += 1;
                else
                    vec.push_back({x, x});
            }

            if (not vec.empty())
                data_by_y[y] = vec;
        }

        std::map<int, std::vector<std::pair<int, int>>> data_by_x;
        for (int x = _x_min; x <= _x_max; ++x)
        {
            std::vector<std::pair<int, int>> vec = {};
            for (int y = _y_min; y <= _y_max; ++y)
            {
                auto it = _data.find({x, y});
                if (it == _data.end())
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

        for (int x = _x_min; x <= _x_max; ++x)
        {
            auto it = data_by_x.find(x);
            if (it == data_by_x.end())
                continue;

            for (auto& range : it->second)
            {
                int y1 = range.first;
                int y2 = range.second;
                _outline_vertices.left_to_right.push_back({{x, y1}, {x+1, y1}});
                _outline_vertices.right_to_left.push_back({{x, y2+1}, {x+1, y2+1}});
            }
        }

        for (int y = _y_min; y <= _y_max; ++y)
        {
            auto it = data_by_y.find(y);
            if (it == data_by_y.end())
                continue;

            for (auto& range : it->second)
            {
                int x1 = range.first;
                int x2 = range.second;
                _outline_vertices.bottom_to_top.push_back({{x1, y}, {x1, y+1}});
                _outline_vertices.top_to_bottom.push_back({{x2+1, y}, {x2+1, y+1}});
            }
        }
    }

    size_t Selection::size() const
    {
        return _data.size();
    }
}