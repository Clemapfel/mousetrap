// 
// Copyright 2022 Clemens Cords
// Created on 8/15/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <map>
#include <vector>
#include <list>

#include <include/colors.hpp>

#include <app/palette.hpp>

namespace mousetrap::state
{
    struct Layer {};
    struct Frame {};

    using namespace mousetrap;

    // ### COLORS #####################################

    HSVA primary_color,
         secondary_color;

    // ### PALETTES ###################################

    using PaletteID = std::string;

    std::map<PaletteID, Palette> palettes = {
        {"debug", debug_palette}
    };

    PaletteID default_palette_id = "debug";
    PaletteID active_palette_id = "debug";

    // ### FRAMES / LAYERS ############################
}

namespace mousetrap
{
    void set_primary_color(HSVA color)
    {
        state::primary_color = color;
    }
}