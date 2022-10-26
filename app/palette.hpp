
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
                non_grayscale.push_back(pair.second);

        std::sort(grayscale.begin(), grayscale.end(), [](HSVA a, HSVA b) -> bool {
            return a.v < b.v;
        });

        std::sort(non_grayscale.begin(), non_grayscale.end(), [](HSVA a, HSVA b) -> bool {
           return a.h < b.h;
        });

        std::vector<HSVA> out;
        for (auto& c : non_grayscale)
            out.push_back(c);

        for (auto& c : grayscale)
            out.push_back(c);

        return out;
    }

    std::vector<HSVA> Palette::get_colors_by_saturation()
    {
        auto out = get_colors_by_hue();
        std::sort(out.begin(), out.end(), [](HSVA a, HSVA b) -> bool {
            if (a.s == b.s)
            {
                if (a.v == b.v)
                    return a.h > b.h;
                else
                    return a.v > b.v;
            }
            else
                return a.s > b.s;
        });

        return out;
    }

    std::vector<HSVA> Palette::get_colors_by_value()
    {
        auto out = get_colors_by_hue();
        std::sort(out.begin(), out.end(), [](HSVA a, HSVA b) -> bool {
            if (a.v == b.v)
            {
                if (a.s == b.s)
                    return a.h < b.h;
                else
                    return a.s > b.s;
            }
            else
                return a.v > b.v;
        });

        return out;
    }
}

