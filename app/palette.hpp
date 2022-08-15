
//
// Copyright 2022 Clemens Cords
// Created on 8/15/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/colors.hpp>

#include <vector>
#include <map>
#include <algorithm>

namespace mousetrap
{
    class Palette
    {
        public:
            Palette() = default;
            Palette(const std::vector<HSVA>&);

            void load_from(const std::string& file);
            void save_to(const std::string& file);

            std::vector<HSVA> get_colors();
            std::vector<HSVA> get_colors_by_hue();
            std::vector<HSVA> get_colors_by_value();
            std::vector<HSVA> get_colors_by_saturation();

        private:
            std::map<size_t, HSVA> _colors;
    };

    static inline const Palette debug_palette = []() -> Palette
    {
        std::vector<HSVA> colors;

        const size_t n_steps = 16;
        for (size_t i = 0; i < n_steps; ++i)
        {
            float h = i / float(n_steps);

            for (float s : {0.33f, 0.66f, 1.f})
                for (float v : {0.33f, 0.66f, 1.f})
                    colors.push_back(HSVA(h, s, v, 1));
        }

        for (size_t i = 0; i < n_steps; ++i)
            colors.push_back(HSVA(0, 1, i / float(n_steps), 1));

        return Palette(colors);
    }();
}

// ###

namespace mousetrap
{
    Palette::Palette(const std::vector<HSVA>& vec)
    {
        for (size_t i = 0; i < vec.size(); ++i)
            _colors.insert({i, vec.at(i)});
    }

    void Palette::load_from(const std::string& file)
    {
        std::cerr << "[ERROR] In Palette::load_from: TODO" << std::endl;
    }

    void Palette::save_to(const std::string& file)
    {
        std::cerr << "[ERROR] In Palette::save_to: TODO" << std::endl;
    }

    std::vector<HSVA> Palette::get_colors()
    {
        std::vector<HSVA> out;
        for (auto& pair : _colors)
            out.push_back(pair.second);

        return out;
    }

    std::vector<HSVA> Palette::get_colors_by_hue()
    {
        auto grayscale = std::vector<HSVA>();
        auto non_grayscale = std::vector<HSVA>();

        for (auto& pair : _colors)
            if (pair.second.s == 0)
                grayscale.push_back(pair.second);
            else
                grayscale.push_back(pair.second);

        std::sort(grayscale.begin(), grayscale.end(), [](HSVA a, HSVA b) -> bool {
            return a.v < b.v;
        });

        std::sort(non_grayscale.begin(), non_grayscale.end(), [](HSVA a, HSVA b) -> bool {
           return a.h < b.h;
        });

        std::vector<HSVA> out;
        for (auto& c : grayscale)
            out.push_back(c);

        for (auto& c : non_grayscale)
            out.push_back(c);

        return out;
    }

    std::vector<HSVA> Palette::get_colors_by_saturation()
    {
        auto out = get_colors_by_hue();
        std::sort(out.begin(), out.end(), [](HSVA a, HSVA b) -> bool {
            return a.s < b.s;
        });

        return out;
    }

    std::vector<HSVA> Palette::get_colors_by_value()
    {
        auto out = get_colors_by_hue();
        std::sort(out.begin(), out.end(), [](HSVA a, HSVA b) -> bool {
            return a.v < b.v;
        });

        return out;
    }
}

