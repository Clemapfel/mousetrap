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
#include <app/brush.hpp>
#include <app/tools.hpp>
#include <app/settings_files.hpp>

namespace mousetrap::state
{
    using namespace mousetrap;

    // ### GTK INSTANCES ##############################

    Window* main_window = nullptr;
    Application* app = nullptr;
    MenuModel* global_menu_bar_model = nullptr;

    // ### COLORS #####################################

    HSVA primary_color = RGBA(1, 0, 1, 1).operator HSVA();
    HSVA secondary_color = RGBA(1, 1, 0, 1).operator HSVA();
    
    size_t color_quantization = 1000;

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

    Brush* current_brush = nullptr;
    size_t brush_size = 1;
    float brush_opacity = 1;

    std::vector<Brush*> brushes;

    Vector2Set<int> selection;

    // ### LAYERS #####################################

    std::deque<Layer*> layers;
    Vector2ui layer_resolution;
    size_t n_frames = 0;

    size_t current_frame = 2;
    size_t current_layer = 0;

    AppComponent* layer_view = nullptr;

    void update_layer_view()
    {
        if (layer_view != nullptr)
            layer_view->update();
    }

    AppComponent* frame_view = nullptr;

    void update_frame_view()
    {
        if (frame_view != nullptr)
            frame_view->update();
    }

    // ### RENDER DRAW #################################

    AppComponent* canvas;

    void update_canvas()
    {
        if (canvas != nullptr)
            canvas->update();
    }

    // ### SYSTEM ######################################

    AppComponent* bubble_log;
    void validate_keybindings_file();

    // ### FORMATING ###################################

    float margin_unit = 10; // px
    size_t slider_min_width = 0;
    size_t icon_scale = 1;
}