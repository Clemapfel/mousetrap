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
            std::string get_name();

        private:
            Texture* _texture = nullptr;
            std::string _name;

            void generate_outline_vertices(Image&);
            std::deque<Vector2i> _outline_vertices;
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
        const auto size = image.get_size();
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

    std::string Brush::get_name()
    {
        return _name;
    }

    void Brush::generate_outline_vertices(Image& image)
    {
        auto w = image.get_size().x;
        auto h = image.get_size().y;

        float alpha_eps = 0.00001;
        std::deque<Vector2i> vertices;

        // get all vertex candidates

        std::set<int> already_pushed_x;
        std::set<int> already_pushed_y;

        auto push_vertex = [&](Vector2i v) -> void
        {
            if (already_pushed_x.find(v.x) != already_pushed_x.end() and
                already_pushed_y.find(v.y) != already_pushed_y.end())
                vertices.push_back(v);
        };

        for (size_t x = 0; x < w; ++x)
        {
            for (size_t y = 0; y < h; ++y)
            {
                if (image.get_pixel(x, y).a < alpha_eps)
                    continue;

                // is neighbor pixel part of shape
                bool top = y > 0 and image.get_pixel(x, y - 1).a > alpha_eps;
                bool right = x < w-1 and image.get_pixel(x + 1, y).a > alpha_eps;
                bool bottom = y < h-1 and image.get_pixel(x, y + 1).a > alpha_eps;
                bool left = x > 0 and image.get_pixel(x - 1, y).a > alpha_eps;

                if (left or top)
                    push_vertex({x, y});

                if (top or right)
                    push_vertex({x + 1, y});

                if (right or bottom)
                    push_vertex({x + 1, y + 1});

                if (bottom or left)
                    push_vertex({x, y + 1});
            }
        }

        _outline_vertices = vertices;
    }
}