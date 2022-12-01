// 
// Copyright 2022 Clemens Cords
// Created on 12/1/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <unordered_set>
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

        private:
            struct PixelCompare
            {
                bool operator()(const Pixel& a, const Pixel& b) const
                {
                    return a.x == b.x and a.y == b.y;
                }
            };

            std::unordered_set<Pixel, PixelCompare> _data;
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
}