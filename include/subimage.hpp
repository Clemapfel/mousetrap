// 
// Copyright 2022 Clemens Cords
// Created on 12/1/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <set>
#include <vector>

#include <include/colors.hpp>

namespace mousetrap
{
    /// \brief set of pixels, not necessarily square or connected
    class SubImage
    {
        public:

            struct Pixel
            {
                Pixel(int x, int y, RGBA);
                Pixel(Vector2i xy, RGBA);

                int x;
                int y;
                RGBA color;
            };

            SubImage();

            void add(Vector2i, RGBA);
            void add(const std::vector<Vector2i>&, RGBA);
            void add(const std::vector<SubImage::Pixel>&);

            auto begin();
            auto begin() const;
            auto end();
            auto end() const;

            size_t get_n_pixels() const;

        private:
            struct PixelCompare
            {
                bool operator()(const Pixel& a, const Pixel& b) const
                {
                    if (a.x == b.x)
                        return a.y < b.y;
                    else
                        return a.x < b.x;
                }
            };

            std::set<Pixel, PixelCompare> _data;
    };
}

//

namespace mousetrap
{
    SubImage::Pixel::Pixel(int x, int y, RGBA color)
        : x(x), y(y), color(color)
    {}

    SubImage::Pixel::Pixel(Vector2i xy, RGBA color)
        : x(xy.x), y(xy.y), color(color)
    {}

    SubImage::SubImage()
    {}

    void SubImage::add(Vector2i pos, RGBA color)
    {
        _data.insert(Pixel(pos, color));
    }

    void SubImage::add(const std::vector<Vector2i>& positions, RGBA color)
    {
        for (auto& pos : positions)
            add(pos, color);
    }

    void SubImage::add(const std::vector<SubImage::Pixel>& pixels)
    {
        for (auto& pixel : pixels)
            _data.insert(pixel);
    }

    size_t SubImage::get_n_pixels() const
    {
        return _data.size();
    }

    auto SubImage::begin()
    {
        return _data.begin();
    }

    auto SubImage::begin() const
    {
        return _data.begin();
    }

    auto SubImage::end()
    {
        return _data.end();
    }

    auto SubImage::end() const
    {
        return _data.end();
    }
}