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
#include <app/layer.hpp>

namespace mousetrap::state
{
    using namespace mousetrap;

    // ### GTK INSTANCES ##############################

    Window* main_window = nullptr;
    Application* app = nullptr;
    MenuModel* global_menu_bar_model = nullptr;

    ShortcutController* shortcut_controller = nullptr;

    // ### COLORS #####################################

    HSVA primary_color = RGBA(1, 0, 1, 1).operator HSVA();
    HSVA secondary_color = RGBA(1, 1, 0, 1).operator HSVA();

    Palette palette;
    PaletteSortMode palette_sort_mode = PaletteSortMode::NONE;

    HSVA preview_color_current = primary_color;
    HSVA preview_color_previous = primary_color;

    // ### TOOLS ######################################

    AppComponent* toolbox = nullptr;
    ToolID active_tool = BRUSH;

    Brush* current_brush = nullptr;
    size_t brush_size = 1;
    float brush_opacity = 1;

    std::vector<Brush*> brushes;
    Vector2Set<int> selection;

    // ### LAYERS #####################################

    std::deque<mousetrap::Layer*> layers;
    Vector2ui layer_resolution;

    size_t current_layer = 0;

    // ### ANIMATION ###################################

    size_t current_frame = 0;
    size_t n_frames = 0;

    bool playback_active = false;

    bool onionskin_visible = false;
    size_t onionskin_n_layers = 0;

    // ### SYSTEM ######################################

    void validate_keybindings_file();

    // ### FORMATING ###################################

    float margin_unit = 10; // px
    size_t slider_min_width = 0;
    size_t icon_scale = 1;
}