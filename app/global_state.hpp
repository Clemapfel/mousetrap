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
#include <app/tools.hpp>

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

    Widget* toolbox = nullptr;
    ToolID active_tool = BRUSH;

    Widget* brush_options = nullptr;
    Image* brush_texture_image = nullptr;
    Texture* brush_texture = nullptr;

    BrushType brush_type = BrushType::SQUARE;
    size_t brush_size = 1;
    float brush_opacity = 1;

    void update_brush_texture();

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

void mousetrap::state::update_brush_texture()
{
    if (state::brush_texture_image == nullptr)
        state::brush_texture_image = new Image();

    if (state::brush_texture == nullptr)
        state::brush_texture = new Texture();

    size_t size = state::brush_size;
    static auto matrix_to_image = [](std::vector<float> in) {
        for (size_t i = 0; i < in.size(); ++i)
        {
            if (in.at(i) == 0)
                state::brush_texture_image->set_pixel(i, RGBA(0, 0, 0, 0));
            else
                state::brush_texture_image->set_pixel(i, RGBA(1, 1, 1, 1));
        }
    };

    if (state::brush_type == BrushType::SQUARE)
    {
        state::brush_texture_image->create(size, size, RGBA(1, 1, 1, 1));
    }
    else if (state::brush_type == BrushType::CIRCLE)
    {
        state::brush_texture_image->create(size, size, RGBA(0, 0, 0, 0));

        if (size == 1)
        {
            matrix_to_image({
                1
            });
        }
        else if (size == 2)
        {
            matrix_to_image({
                1, 1,
                1, 1
            });
        }
        else if (size == 3)
        {
            matrix_to_image({
                0, 1, 0,
                1, 1, 1,
                0, 1, 0
            });
        }
        else if (size == 4)
        {
            matrix_to_image({
                0, 1, 1, 0,
                1, 1, 1, 1,
                1, 1, 1, 1,
                0, 1, 1, 0
            });
        }
        else if (size == 5)
        {
            matrix_to_image({
                0, 1, 1, 1, 0,
                1, 1, 1, 1, 1,
                1, 1, 1, 1, 1,
                1, 1, 1, 1, 1,
                0, 1, 1, 1, 0
            });
        }
        else if (size == 6)
        {
            matrix_to_image({
                0, 1, 1, 1, 1, 0,
                1, 1, 1, 1, 1, 1,
                1, 1, 1, 1, 1, 1,
                1, 1, 1, 1, 1, 1,
                1, 1, 1, 1, 1, 1,
                0, 1, 1, 1, 1, 0
            });
        }
        else if (size == 7)
        {
            matrix_to_image({
                0, 0, 1, 1, 1, 0, 0,
                0, 1, 1, 1, 1, 1, 0,
                1, 1, 1, 1, 1, 1, 1,
                1, 1, 1, 1, 1, 1, 1,
                1, 1, 1, 1, 1, 1, 1,
                0, 1, 1, 1, 1, 1, 0,
                0, 0, 1, 1, 1, 0, 0
            });
        }
        else if (size == 8)
        {
            matrix_to_image({
                0, 0, 1, 1, 1, 1, 0, 0,
                0, 1, 1, 1, 1, 1, 1, 0,
                1, 1, 1, 1, 1, 1, 1, 1,
                1, 1, 1, 1, 1, 1, 1, 1,
                1, 1, 1, 1, 1, 1, 1, 1,
                1, 1, 1, 1, 1, 1, 1, 1,
                0, 1, 1, 1, 1, 1, 1, 0,
                0, 0, 1, 1, 1, 1, 0, 0
            });
        }
        else
        {
            auto center = size / 2 + (size % 2 == 0 ? -0.5 : 0);
            auto radius = size / 2 + (size % 2 == 0 ? 1 : 1);

            for (size_t x = 0; x < size; ++x)
                for (size_t y = 0; y < size; ++y)
                    if (glm::distance(Vector2f(x, y), Vector2f(center)) < radius)
                        state::brush_texture_image->set_pixel(x, y, RGBA(1, 1, 1, 1));

        }
    }
    else if (state::brush_type == BrushType::RECTANGLE_HORIZONTAL)
    {
        state::brush_texture_image->create(size, size, RGBA(1, 1, 1, 1));

        if (size == 1)
        {
            matrix_to_image({1});
        }
        else if (size == 2)
        {
            matrix_to_image({1, 1, 1, 1});
        }
        else if (size == 4)
        {
            matrix_to_image({
                0, 0, 0, 0,
                1, 1, 1, 1,
                1, 1, 1, 1,
                0, 0, 0, 0
            });
        }
        else
        {
            for (size_t x = 0; x < size; ++x)
                for (size_t y = 0; y < size; ++y)
                        state::brush_texture_image->set_pixel(x, y, RGBA(0, 0, 0, 0));
        }
    }
    else if (state::brush_type == BrushType::RECTANGLE_VERTICAL)
    {
        std::cerr << "[ERROR] In state::update_brush_texture: RECTANGLE_VERTICAL TODO" << std::endl;
    }
    else if (state::brush_type == BrushType::ELLIPSE_HORIZONTAL)
    {
        std::cerr << "[ERROR] In state::update_brush_texture: ELLIPSE_HORIZONTAL TODO" << std::endl;
    }
    else if (state::brush_type == BrushType::ELLIPSE_VERTICAL)
    {
        std::cerr << "[ERROR] In state::update_brush_texture: ELLIPSE_VERTICAL TODO" << std::endl;
    }
    else if (state::brush_type == BrushType::LINE_HORIZONTAL)
    {
        state::brush_texture_image->create(size, size, RGBA(0, 0, 0, 0));

        if (size == 1)
            matrix_to_image({1});
        else
        {
            if (size % 2 == 1)
            {
                for (size_t x = 0; x < size; ++x)
                    state::brush_texture_image->set_pixel(x, size / 2, RGBA(1, 1, 1, 1));
            }
            else
            {
                for (size_t x = 0; x < size; ++x)
                {
                    state::brush_texture_image->set_pixel(x, size / 2 - 1, RGBA(1, 1, 1, 1));
                    //state::brush_texture_image->set_pixel(x, size / 2, RGBA(1, 1, 1, 1));
                }
            }
        }
    }
    else if (state::brush_type == BrushType::LINE_VERTICAL)
    {
        state::brush_texture_image->create(size, size, RGBA(0, 0, 0, 0));

        if (size == 1)
            matrix_to_image({1});
        else
        {
            if (size % 2 == 1)
            {
                for (size_t y = 0; y < size; ++y)
                    state::brush_texture_image->set_pixel(size / 2, y, RGBA(1, 1, 1, 1));
            }
            else
            {
                for (size_t y = 0; y < size; ++y)
                {
                    state::brush_texture_image->set_pixel(size / 2 - 1, y, RGBA(1, 1, 1, 1));
                    //state::brush_texture_image->set_pixel(size / 2, y, RGBA(1, 1, 1, 1));
                }
            }
        }
    }

    state::brush_texture->create_from_image(*state::brush_texture_image);
}