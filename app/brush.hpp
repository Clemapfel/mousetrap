// 
// Copyright 2022 Clemens Cords
// Created on 11/25/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap.hpp>

namespace mousetrap
{
    class Brush
    {
        public:
            Brush();
            ~Brush();

            void create_from_image(Image&, const std::string& name);
            void create_from_file(const std::string& path);

            Texture* get_texture();
            const std::string& get_name();
            const std::deque<std::pair<Vector2i, Vector2i>>& get_outline_vertices();

        private:
            static inline const float alpha_eps = 0.00001; // lowest alpha value that is still registered as non-transparent

            Texture* _texture = nullptr;
            std::string _name;

            void generate_outline_vertices(Image&);
            std::deque<std::pair<Vector2i, Vector2i>> _outline_vertices;
            // vertex position is top left of pixel coord, where top left of texture is (0, 0)
    };
}

//

namespace mousetrap
{
    Brush::Brush()
    {}

    Brush::~Brush()
    {
        delete _texture;
    }

    void Brush::create_from_file(const std::string& path)
    {
        auto image = Image();
        if (not image.create_from_file(path))
        {
            std::cerr << "[WARNING] Unable to load brush at `" << path << "`: Image.create_from_file failed"
                      << std::endl;
            return;
        }

        if (image.get_size().x != image.get_size().y)
        {
            std::cerr << "[WARNING] Unable to load brush at `" << path << "`: Brush textures have to be square"
                      << std::endl;

            return;
        }

        size_t extension_size = 0;
        for (auto it = path.end(); it != path.begin(); it--)
        {
            extension_size += 1;

            if (*it == '.')
                break;
        }

        std::stringstream name;
        for (size_t i = 0; i < path.size() - extension_size; ++i)
        {
            auto c = path.at(i);
            if (c == '_')
                name << ' ';
            else
                name << c;
        }

        create_from_image(image, name.str());
    }

    void Brush::create_from_image(Image& image, const std::string& name)
    {
        auto size = image.get_size();

        static bool once = true;
        for (size_t x = 0; x < size.x; ++x)
        {
            for (size_t y = 0; y < size.y; ++y)
            {
                RGBA color = image.get_pixel(x, y);

                auto as_hsva = color.operator HSVA();
                color.r = as_hsva.v;
                color.g = as_hsva.v;
                color.b = as_hsva.v;

                if (as_hsva.v < 0.05 or color.a < 0.05)
                    color.a = 0;
                else
                    color.a = 1;

                image.set_pixel(x, y, color);
            }
        }

        generate_outline_vertices(image);

        _texture = new Texture();
        _texture->create_from_image(image);

        _name = name;
    }

    Texture* Brush::get_texture()
    {
        return _texture;
    }

    const std::string& Brush::get_name()
    {
        return _name;
    }

    const std::deque<std::pair<Vector2i, Vector2i>>& Brush::get_outline_vertices()
    {
        return _outline_vertices;
    }

    void Brush::generate_outline_vertices(Image& image)
    {
        auto w = image.get_size().x;
        auto h = image.get_size().y;

        /*
        // get all vertex candidates
        struct Vector2iCompare
        {
            bool operator()(Vector2i a, Vector2i b) const {
                if (a.x < b.x)
                    return true;
                else
                    return (a.y < b.y);
            }
        };

        std::set<Vector2i, Vector2iCompare> vertices;

        // add all candidates
        for (size_t x = 0; x < w; ++x)
        {
            for (size_t y = 0; y < h; ++y)
            {
                if (image.get_pixel(x, y).a > alpha_eps)
                {
                    vertices.insert({x, y});
                    vertices.insert({x + 1, y});
                    vertices.insert({x + 1, y + 1});
                    vertices.insert({x, y + 1});
                }
            }
        }

        auto matrix = std::vector<std::vector<bool>>();
        matrix.resize(w+1);

        for (auto& row : matrix)
            row.resize(h+1);

        // purge non-bounding vertices
        for (const auto& v : vertices)
        {
            auto left_border = v.x == 0;
            auto top_border = v.y == 0;
            auto right_border = v.x == w;
            auto bottom_border = v.y == h;

            if (left_border or top_border or right_border or bottom_border)
            {
                matrix[v.x][v.y] = true;
                continue;
            }

            auto top_left = (v.x == 0 and v.y == 0) or image.get_pixel(v.x - 1, v.y - 1).a > alpha_eps;
            auto top_right = v.y == 0 or image.get_pixel(v.x, v.y - 1).a > alpha_eps;
            auto bottom_right = image.get_pixel(v.x, v.y).a > alpha_eps;
            auto bottom_left = v.x == 0 or image.get_pixel(v.x - 1, v.y).a > alpha_eps;

            if (not (top_left and top_right and bottom_right and bottom_left))
                matrix[v.x][v.y] = true;
        }

        auto debug_print = [&]()
        {
            std::cout << std::endl;
            for (size_t x = 0; x < w + 1; ++x)
            {
                for (size_t y = 0; y < h + 1; ++y)
                    std::cout << matrix[x][y] << " ";

                std::cout << std::endl;
            }
        };
        */

        _outline_vertices.clear();

        // hlines
        for (size_t x = 0; x < w; ++x)
            for (size_t y = 0; y < h-1; ++y)
                if (image.get_pixel(x, y).a > alpha_eps != image.get_pixel(x, y+1).a > alpha_eps)
                    _outline_vertices.push_back({{x, y+1}, {x+1, y+1}});

        for (size_t x = 0; x < w; ++x)
            if (image.get_pixel(x, 0).a > alpha_eps)
                _outline_vertices.push_back({{x, 0}, {x+1, 0}});

        for (size_t x = 0; x < w; ++x)
            if (image.get_pixel(x, h-1).a > alpha_eps)
                _outline_vertices.push_back({{x, h}, {x+1, h}});

        // vlines
        for (size_t y = 0; y < h; ++y)
            for (size_t x = 0; x < w-1; ++x)
                if (image.get_pixel(x, y).a > alpha_eps != image.get_pixel(x+1, y).a > alpha_eps)
                    _outline_vertices.push_back({{x+1, y}, {x+1, y+1}});

        for (size_t y = 0; y < h; ++y)
            if (image.get_pixel(0, y).a > alpha_eps)
                _outline_vertices.push_back({{0, y}, {0, y+1}});

        for (size_t y = 0; y < h; ++y)
            if (image.get_pixel(w-1, y).a > alpha_eps)
                _outline_vertices.push_back({{w, y}, {w, y+1}});
    }
}