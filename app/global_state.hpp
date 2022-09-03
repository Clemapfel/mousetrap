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
#include <include/shortcut_map.hpp>

#include <app/palette.hpp>

namespace mousetrap::state
{
    using namespace mousetrap;

    // ### GTK INSTANCES ##############################

    Window* main_window = nullptr;
    Application* app = nullptr;
    ShortcutMap* shortcut_map = nullptr;

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

    // ### TOOLS ######################################

    enum ToolboxTool
    {
        MARQUEE_RECTANGLE,
        MARQUEE_CIRCLE,
        MARQUEE_POLYGON,
        MARQUEE_NEIGHBORHOOD_SELECT,
        MARQUEE_BRUSH,
        MARQUEE_FILL,

        BRUSH,
        ERASER,
        EYEDROPPER,
        BUCKET_FILL,

        LINE_SHAPE,
        RECTANGLE_SHAPE_OUTLINE,
        RECTANGLE_SHAPE_FILL,
        CIRCLE_SHAPE_OUTLINE,
        CIRCLE_SHAPE_FILL,

        GRADIENT_SMOOTH,
        GRADIENT_DITHERED
    };

    ToolboxTool active_tool = BRUSH;

    enum class MarqueeMode
    {
        REPLACE,
        ADD,
        SUBTRACT
    };

    MarqueeMode marquee_mode = MarqueeMode::REPLACE;

    enum class BrushType
    {
        RECTANGLE,
        CIRCLE,
        CUSTOM
    };

    BrushType brush_type = BrushType::RECTANGLE;

    size_t brush_size = 1; // rectangle size

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