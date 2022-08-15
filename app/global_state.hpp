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

    HSVA primary_color = HSVA(0.3, 1, 1, 1);
    HSVA secondary_color = HSVA(0.31, 1, 1, 1);

    // ### PALETTES ###################################

    using PaletteID = std::string;

    std::map<PaletteID, Palette> palettes = {
        {"debug", debug_palette}
    };

    PaletteID default_palette_id = "debug";
    PaletteID active_palette_id = "debug";

    // ### FRAMES / LAYERS ############################

    // ### FORMATING ##################################

    float margin_unit = 10; //px
}

// ###

namespace mousetrap
{
    void set_primary_color(HSVA color)
    {
        state::primary_color = color;
    }

    void set_secondary_color(HSVA color)
    {
        state::secondary_color = color;
    }
}