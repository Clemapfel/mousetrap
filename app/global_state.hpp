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
#include <include/window.hpp>
#include <include/pixel_buffer.hpp>

#include <app/palette.hpp>

namespace mousetrap::state
{
    using namespace mousetrap;

    // ### GTK INSTANCES ##############################

    Window* main_window = nullptr;

    // ### COLORS #####################################

    HSVA primary_color = RGBA(1, 0, 1, 1).operator HSVA();
    HSVA secondary_color = RGBA(0, 1, 0, 1).operator HSVA();

    Widget* color_picker = nullptr;
    Widget* primary_secondary_color_swapper = nullptr;
    Widget* verbose_color_picker = nullptr;
    Widget* palette_view = nullptr;

    void set_primary_color(HSVA color)
    {
        primary_color = color;
        color_picker->update();
        primary_secondary_color_swapper->update();
    }

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