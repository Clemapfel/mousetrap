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
#include <include/undo_stack.hpp>
#include <include/ring_layout.hpp>

#include <app/layer.hpp>
#include <app/app_component.hpp>
#include <app/palette.hpp>
#include <app/tools.hpp>

namespace mousetrap::state
{
    using namespace mousetrap;

    // ### GTK INSTANCES ##############################

    Window* main_window = nullptr;
    Application* app = nullptr;
    MenuModel* global_menu_bar_model = nullptr;
    ConfigFile* settings_file = nullptr;
    ConfigFile* keybindings_file = nullptr;
    ConfigFile* tooltips_file = nullptr;

    // ### COLORS #####################################

    HSVA primary_color = RGBA(1, 0, 1, 1).operator HSVA();
    HSVA secondary_color = RGBA(1, 1, 0, 1).operator HSVA();

    AppComponent* color_picker = nullptr;
    AppComponent* color_swapper = nullptr;
    AppComponent* verbose_color_picker = nullptr;
    AppComponent* palette_view = nullptr;

    HSVA preview_color_current = primary_color;
    HSVA preview_color_previous = primary_color;
    AppComponent* color_preview = nullptr;

    void update_color_picker()
    {
        if (color_picker != nullptr)
            color_picker->update();
    }

    void update_color_swapper()
    {
        if (color_swapper != nullptr)
            color_swapper->update();
    }

    void update_verbose_color_picker()
    {
        if (verbose_color_picker != nullptr)
            verbose_color_picker->update();
    }

    void update_palette_view()
    {
        if (palette_view != nullptr)
            palette_view->update();
    }

    void update_color_preview()
    {
        if (color_preview != nullptr)
            color_preview->update();
    }

    // ### TOOLS ######################################

    AppComponent* toolbox = nullptr;
    ToolID active_tool = BRUSH;

    AppComponent* brush_options = nullptr;
    Image* brush_texture_image = nullptr;
    Texture* brush_texture = nullptr;

    BrushType brush_type = BrushType::SQUARE;
    size_t brush_size = 1;
    float brush_opacity = 1;

    void update_brush_texture();

    // ### LAYERS #####################################

    std::deque<Layer*> layers;
    Vector2ui layer_resolution;
    size_t n_frames = 0;

    size_t current_frame = 2;
    size_t current_layer = 0;

    AppComponent* layer_view = nullptr;

    // ### RENDER DRAW #################################

    AppComponent* canvas;

    // ### SYSTEM ######################################



    // ### FORMATING ###################################

    float margin_unit = 10; // px
    size_t slider_min_width = 0;
    size_t icon_scale = 1;
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
        state::brush_texture_image->create(size, size, RGBA(0, 0, 0, 0));

        if (size == 1)
        {
            matrix_to_image({1});
        }
        else if (size == 2)
        {
            matrix_to_image({1, 1, 1, 1});
        }
        else if (size == 3)
        {
            matrix_to_image({
                0, 0, 0,
                1, 1, 1,
                0, 0, 0,
            });
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
        else if (size == 5)
        {
            matrix_to_image({
                0, 0, 0, 0, 0,
                1, 1, 1, 1, 1,
                1, 1, 1, 1, 1,
                1, 1, 1, 1, 1,
                0, 0, 0, 0, 0
            });
        }
        else
        {
            size_t n = size - (size % 2);
            size_t n_lines = n / 2;
            size_t start = size / 2 - n_lines / 2;

            for (size_t x = 0; x < size; ++x)
                for (size_t y = start; y < start + n_lines; ++y)
                    state::brush_texture_image->set_pixel(x, y, RGBA(1, 1, 1, 1));
        }
    }
    else if (state::brush_type == BrushType::RECTANGLE_VERTICAL)
    {
        state::brush_texture_image->create(size, size, RGBA(0, 0, 0, 0));

        if (size == 1)
        {
            matrix_to_image({1});
        }
        else if (size == 2)
        {
            matrix_to_image({1, 1, 1, 1});
        }
        else if (size == 3)
        {
            matrix_to_image({
                    0, 0, 0,
                    1, 1, 1,
                    0, 0, 0,
            });
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
        else if (size == 5)
        {
            matrix_to_image({
                    0, 0, 0, 0, 0,
                    1, 1, 1, 1, 1,
                    1, 1, 1, 1, 1,
                    1, 1, 1, 1, 1,
                    0, 0, 0, 0, 0
            });
        }
        else
        {
            size_t n = size - (size % 2);
            size_t n_lines = n / 2;
            size_t start = size / 2 - n_lines / 2;

            for (size_t y = 0; y < size; ++y)
                for (size_t x = start; x < start + n_lines; ++x)
                    state::brush_texture_image->set_pixel(x, y, RGBA(1, 1, 1, 1));
        }
    }
    else if (state::brush_type == BrushType::ELLIPSE_HORIZONTAL)
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
                    0, 0, 0, 0,
                    1, 1, 1, 1,
                    1, 1, 1, 1,
                    0, 0, 0, 0
            });
        }
        else if (size == 5)
        {
            matrix_to_image({
                    0, 0, 0, 0, 0,
                    0, 1, 1, 1, 0,
                    1, 1, 1, 1, 1,
                    0, 1, 1, 1, 0,
                    0, 0, 0, 0, 0
            });
        }
        else if (size == 6)
        {
            matrix_to_image({
                    0, 0, 0, 0, 0, 0,
                    0, 1, 1, 1, 1, 0,
                    1, 1, 1, 1, 1, 1,
                    1, 1, 1, 1, 1, 1,
                    0, 1, 1, 1, 1, 0,
                    0, 0, 0, 0, 0, 0
            });
        }
        else if (size == 7)
        {
            matrix_to_image({
                    0, 0, 0, 0, 0, 0, 0,
                    0, 0, 1, 1, 1, 0, 0,
                    0, 1, 1, 1, 1, 1, 0,
                    1, 1, 1, 1, 1, 1, 1,
                    0, 1, 1, 1, 1, 1, 0,
                    0, 0, 1, 1, 1, 0, 0,
                    0, 0, 0, 0, 0, 0, 0
            });
        }
        else if (size == 8)
        {
            matrix_to_image({
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 1, 1, 0, 0, 0,
                    0, 1, 1, 1, 1, 1, 1, 0,
                    1, 1, 1, 1, 1, 1, 1, 1,
                    1, 1, 1, 1, 1, 1, 1, 1,
                    0, 1, 1, 1, 1, 1, 1, 0,
                    0, 0, 0, 1, 1, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0
            });
        }
        else
        {
            auto center = Vector2f(size / 2 + (size % 2 == 0 ? -0.5 : 0));
            auto radius = size / 2 + (size % 2 == 0 ? 1 : 1);

            for (size_t x = 0; x < size; ++x)
                for (size_t y = 0; y < size; ++y)
                {
                    if (glm::distance(Vector2f(x, y), center) < radius)
                        state::brush_texture_image->set_pixel(x, y, RGBA(1, 1, 1, 1));
                }
        }
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