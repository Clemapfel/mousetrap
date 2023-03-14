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
            bool is_empty() const;

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

#include <src/subimage.inl>