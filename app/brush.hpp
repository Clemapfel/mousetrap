// 
// Copyright 2022 Clemens Cords
// Created on 11/25/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap.hpp>

#include <app/settings_files.hpp>
#include <app/algorithms.hpp>

namespace mousetrap
{
    class Brush
    {
        public:
            Brush();
            ~Brush();

            void create_from_image(Image&, const std::string& name);
            bool create_from_file(const std::string& path);

            Texture* get_texture();
            const Image& get_base_image();
            const Image& get_image();

            const std::string& get_name();

            size_t get_base_size() const;

            /// \brief update texture as scaled version of base image
            void set_size(size_t px);
            size_t get_size();

            const OutlineVertices& get_outline_vertices() const;

        private:
            float alpha_eps = state::settings_file->get_value_as<float>("global", "alpha_epsilon");

            size_t _size;
            Image _image;
            Image _image_scaled;
            Texture* _texture = nullptr;
            std::string _name;

            void regenerate_outline_vertices(Image&);
            OutlineVertices _outline_vertices;
            // vertex position is top left of pixel coord, where top left of texture is (0, 0)
    };

    void reload_brushes();
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

    bool Brush::create_from_file(const std::string& path)
    {
        auto image = Image();
        if (not image.create_from_file(path))
        {
            std::cerr << "[WARNING] Unable to load brush at `" << path << "`: Image.create_from_file failed"
                      << std::endl;
            return false;
        }

        FileDescriptor file;
        file.create_for_path(path);

        std::stringstream name;
        for (size_t i = 0; i < file.get_name().size() - file.get_extension().size(); ++i)
            name << file.get_name().at(i);

        create_from_image(image, name.str());
        return true;
    }

    void Brush::create_from_image(Image& image, const std::string& name)
    {
        _image = image;
        auto size = image.get_size();

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

        regenerate_outline_vertices(image);

        _texture = new Texture();
        _texture->create_from_image(image);

        _name = name;
        _size = get_base_size();
    }

    Texture* Brush::get_texture()
    {
        return _texture;
    }

    const Image& Brush::get_base_image()
    {
        return _image;
    }

    const Image& Brush::get_image()
    {
        if (_size == get_base_size())
            return _image;
        else
            return _image_scaled;
    }

    const std::string& Brush::get_name()
    {
        return _name;
    }

    void Brush::set_size(size_t px)
    {
        if (_size == px)
            return;

        _size = px;

        int w = _image.get_size().x;
        int h = _image.get_size().y;

        size_t new_w, new_h;

        if (w < h)
        {
            new_w = px;
            new_h = new_w + std::abs(h - w);
        }
        else if (h > w)
        {
            new_h = px;
            new_w = new_h + std::abs(w - h);
        }
        else
        {
            new_w = px;
            new_h = px;
        }

        _image_scaled = _image.as_scaled(new_w, new_h);

        // prevent brush going invisible because of artifacting
        for (size_t x = 0; x < _image_scaled.get_size().x; ++x)
            for (size_t y = 0; y < _image_scaled.get_size().y; ++y)
                if (_image_scaled.get_pixel(x, y).a > alpha_eps)
                    goto skip_artifact_fix;

        for (size_t i = 0; i < _image_scaled.get_n_pixels(); ++i)
            _image_scaled.set_pixel(i, RGBA(1, 1, 1, 1));

        skip_artifact_fix:

        _texture->create_from_image(_image_scaled);
        regenerate_outline_vertices(_image_scaled);
    }

    size_t Brush::get_base_size() const
    {
        return std::min(_image.get_size().x, _image.get_size().y);
    }

    void Brush::regenerate_outline_vertices(Image& image)
    {
        _outline_vertices = generate_outline_vertices(image);
    }

    const OutlineVertices& Brush::get_outline_vertices() const
    {
        return _outline_vertices;
    }
}