//
// Copyright 2022 Clemens Cords
// Created on 8/7/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/image.hpp>
#include <include/texture.hpp>

namespace mousetrap
{
    // 1-d floating point, square texture
    class Brush
    {
        public:
            Brush(size_t size);

            template<typename... Floats> requires (std::is_same_v<Floats, float> && ...)
            Brush(Floats...);

            Brush(std::vector<float> values);

            std::string to_string();
            void create_from_string(const std::string&);

            Image to_image();
            void create_from_image(const Image&); // will only use alpha component

            void as_square(size_t size);
            void as_circle(size_t size);
            void as_smooth_circle(size_t size);

        private:
            static inline const std::string begin = "Brush(";
            static inline const std::string end = ")";

            size_t _width = 0, _height = 0;
            std::vector<float> _data; // x in [0, 1] where each pixel has the color RGBA(0, 0, 0, x)
    };
}

// ###

#include <sstream>

namespace mousetrap
{
    Brush::Brush(size_t size)
        : _width(size), _height(size)
    {
        _data.resize(size * size, 0.f);
    }

    void Brush::create_from_string(const std::string& str)
    {
        // syntax: Brush(0.1, 0.123, 0.142, 0.1511, 0.1)

        std::vector<float> data;
        size_t i = 0;

        if (str.substr(0, begin.size()) != begin)
            goto error;

        if (str.substr(str.size() - end.size()) != end)
            goto error;

        for (i = begin.size(); i < str.size();)
        {
            try
            {
                std::string word = "";
                while (str.at(i) != ',' and i < str.size() - end.size())
                    word.push_back(str.at(i++));

                data.push_back(std::stof(word));
                i += 1;
            }
            catch (...)
            {
                goto error;
            }

            if (i < str.size() - end.size() and str.at(i) == ' ')
                i += 1;
        }

        if (glm::fract(sqrt(data.size())) > 0.0000001)
        {
            std::cerr << "[ERROR] In Brush::create_from_string: trying to read brush with " << data.size() << " element, which cannot be expressed as a square." << std::endl;
            return;
        }

        _data = data;
        _width = sqrt(_data.size());
        _height = _width;
        return;

        error:
            std::cerr << "[ERROR] In Brush::create_from_string: malformated string at position " << i << ": \"" << str.substr(0, i+1) << "\""  << std::endl;
            return;
    }

    std::string Brush::to_string()
    {
        std::stringstream out;
        out << begin;

        if (not _data.empty())
        {
            for (size_t i = 0; i < _data.size() - 1; ++i)
                out << _data.at(i) << ", ";

            out << _data.back();
        }

        out << end;
        return out.str();
    }

    template<typename... Floats> requires (std::is_same_v<Floats, float> && ...)
    Brush::Brush(Floats... in)
    {
        _data = {in ...};
    }

    Brush::Brush(std::vector<float> data)
    {
        _data = data;
    }

    Image Brush::to_image()
    {
        auto out = Image();
        out.create(_width, _height, RGBA(0, 0, 0, 0));

        for (size_t x = 0; x < _width; ++x)
            for (size_t y = 0; y < _height; ++y)
                out.set_pixel(x, y, RGBA(1, 1, 1, _data.at(y * _width + x)));

        return out;
    }

    void Brush::create_from_image(const Image& image)
    {
        if (image.get_size().x != image.get_size().y)
        {
            std::cerr << "[WARNING] In Brush::create_from_image: image is not square, not creating will take place" << std::endl;
            _data = {};
            _width = 0;
            _height = 0;
            return;
        }

        _width = image.get_size().x;
        _height = image.get_size().y;

        for (size_t i = 0; i < _width * _height * 4; i += 4)
            _data.push_back(reinterpret_cast<float*>(image.data())[i + 3]);
    }

    void Brush::as_square(size_t size)
    {
        _width = size+1;
        _height = size+1;

        _data.resize((size + 1) * (size + 1), 0);
        for (size_t x = 1; x < size; ++x)
            for (size_t y = 1; y < size; ++y)
                _data.at(y * size + x) = 0.1;
    }

    void Brush::as_circle(size_t size)
    {
        if (size == 1)
        {
            _data = {
                0, 0, 0,
                0, 1, 0,
                0, 0, 0
            };

            _width = 3, _height = 3;
        }
        else if (size == 2)
        {
            _data = {
                0, 0, 0, 0,
                0, 1, 1, 0,
                0, 1, 1, 0,
                0, 0, 0, 0
            };

            _width = 4, _height = 4;
        }
        else if (size == 3)
        {
            _data = {
                0, 0, 0, 0, 0,
                0, 0, 1, 0, 0,
                0, 1, 1, 1, 0,
                0, 0, 1, 0, 0,
                0, 0, 0, 0, 0
            };

            _width = 5, _height = 5;
        }
        else if (size == 4)
        {
            _data = {
                0, 0, 0, 0, 0, 0,
                0, 0, 1, 1, 0, 0,
                0, 1, 1, 1, 1, 0,
                0, 1, 1, 1, 1, 0,
                0, 0, 1, 1, 0, 0,
                0, 0, 0, 0, 0, 0
            };

            _width = 6, _height = 6;
        }
        else if (size == 5)
        {
            _data = {
                0, 0, 0, 0, 0, 0, 0,
                0, 0, 1, 1, 1, 0, 0,
                0, 1, 1, 1, 1, 1, 0,
                0, 1, 1, 1, 1, 1, 0,
                0, 1, 1, 1, 1, 1, 0,
                0, 0, 1, 1, 1, 0, 0,
                0, 0, 0, 0, 0, 0, 0
            };

            _width = 7, _height = 7;
        }
        else
        {
            size += 2;
            _data.resize(size * size);

            for (size_t x = 0; x < size; ++x)
            {
                for (size_t y = 0; y < size; ++y)
                {
                    float value;

                    if (glm::distance(Vector2f(x / float(size), y / float(size)), Vector2f(0.5, 0.5)) < (size - 1))
                        value = 1;
                    else
                        value = 0;

                    _data.at(y * size + x) = value;
                }
            }

            _width = size, _height = size;
        }
    }
}
