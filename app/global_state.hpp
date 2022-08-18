// 
// Copyright 2022 Clemens Cords
// Created on 8/15/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <map>
#include <vector>
#include <list>
#include <deque>

#include <include/colors.hpp>
#include <include/pixel_buffer.hpp>

#include <app/palette.hpp>

namespace mousetrap::state
{
    using namespace mousetrap;

    // ### COLORS #####################################

    HSVA primary_color = RGBA(1, 0, 1, 0.5).operator HSVA();
    HSVA secondary_color = RGBA(0, 1, 0, 1).operator HSVA();

    // ### PALETTES ###################################

    using PaletteID = std::string;

    std::map<PaletteID, Palette> palettes = {
        {"debug", debug_palette}
    };

    PaletteID default_palette_id = "debug";
    PaletteID active_palette_id = "debug";

    // ### FRAMES / LAYERS ############################

    struct Layer
    {
        bool is_hidden = false;
        bool is_locked = false;

        std::string name;
        float opacity = 1;

        PixelBuffer* pixels = nullptr;
    };

    struct Frame
    {
        std::vector<Layer> layers;
    };

    std::deque<Frame> frames;

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