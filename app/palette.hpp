
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
    enum class PaletteSortMode
    {
        NONE,
        BY_HUE,
        BY_VALUE,
        BY_SATURATION
    };

    std::string palette_sort_mode_to_string(PaletteSortMode mode);
    PaletteSortMode palette_sort_mode_from_string(const std::string&);

    class Palette
    {
        static inline const float truncate_float_after = 10e4;

        public:
            Palette();
            Palette(const std::vector<HSVA>&);

            void set_name(const std::string& name);
            std::string get_name() const;

            bool load_from(const std::string& file);
            bool save_to(const std::string& file) const;
            std::string serialize() const;

            size_t get_n_colors() const;
            std::vector<HSVA> get_colors() const;
            std::vector<HSVA> get_colors_by_hue() const;
            std::vector<HSVA> get_colors_by_value() const;
            std::vector<HSVA> get_colors_by_saturation() const;

            const std::map<size_t, HSVA>& data();

        private:
            std::string _name = "palette";
            std::map<size_t, HSVA> _colors;
    };
}
