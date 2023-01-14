// 
// Copyright 2022 Clemens Cords
// Created on 10/11/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <app/global_state.hpp>

#include <include/menu_model.hpp>
#include <include/application.hpp>

namespace mousetrap::state
{
/*
File >
    New...
    Open...
    Open Recent >
        // names of old files here
    ---
    Save
    Save As...
    Export
    Export As...
    Export As Spritesheet
    ---
    Close
Select >
    Select All
    Invert Selection
    Select Color...
Image >
    Canvas Size...
    Scale
    ---
    Flip Horizontally
    Flip Vertically
    Rotate clockwise
    Rotate counter-clockwise
Colors >
    Palette >
        Load...
        Load Default
        Save...
        Save As Default
        ---
        New...
        New From Image
        Edit...
    ---
    Invert
    Grayscale
    Hue...
    Saturation...
    Value...
    Replace...
    Palette Shift...
Layers / Frames >
    New Layer...
    Duplicate Layer
    Delete Layer
    Merge Layer Down
    Flatten All
    ---
    Hide All Layers
    Show All Layers
    Lock All Layers
    Unlock All Layers
    ---
    New Key Frame
    New Inbetween Frame
    Duplicate Frame
    Delete Frame
View >
    Show Toolbar
    Show Palette
    Show Color Swapper
    Show HSV Color Select
    Show Color Picker
    Show Brush Options
    Show Animation Preview
    Show Layer / Frame View
Settings >
    Keybindings >
        Show...
        Edit...
    Canvas >
        Scrolling... // invert, speed
    Animation >
        Set FPS...
        Set Show Onion Skin...
Other >
    Manual...
    About...
    GitHub
    Donate

*/

    namespace MenuBar
    {
        static MenuModel* _file_menu_model = nullptr;
        static MenuModel* _file_new_create_menu_model = nullptr;
        static MenuModel* _file_save_export_menu_model = nullptr;

        void file_new(void* = nullptr)
        {}

        void file_open(void* = nullptr)
        {}
        
        static inline MenuModel* _file_open_recent_menu_model = nullptr;

        void file_save(void* = nullptr)
        {}

        void file_save_as(void* = nullptr)
        {}

        void file_export(void* = nullptr)
        {}

        void file_export_as(void* = nullptr)
        {}

        void file_export_as_spritesheet(void* = nullptr)
        {}

        void file_close(void* = nullptr)
        {}
        
        static inline MenuModel* _selection_menu_model = nullptr;

        void selection_select_all(void* = nullptr)
        {}

        void selection_invert(void* = nullptr)
        {}

        void selection_select_by_color(void* = nullptr)
        {}

        static inline MenuModel* _image_menu_model;
        static inline MenuModel* _image_size_menu_model;
        static inline MenuModel* _image_transform_menu_model;

        void image_resize_canvas(void* = nullptr)
        {}

        void image_scale_image(void* = nullptr)
        {}

        void image_crop_to_selection(void* = nullptr)
        {}

        void image_transform_flip_horizontally(void* = nullptr)
        {}

        void image_transform_flip_vertically(void* = nullptr)
        {}

        void image_transform_rotate_clockwise(void* = nullptr)
        {}

        void image_transform_rotate_counter_clockwise(void* = nullptr)
        {}

        static inline MenuModel* _colors_menu_model = nullptr;
        static inline MenuModel* _colors_palette_menu_model = nullptr;
        static inline MenuModel* _colors_transform_menu_model = nullptr;

        void colors_palette_load(void* = nullptr)
        {}
        
        void colors_palette_load_default(void* = nullptr)
        {}
        
        void colors_palette_save(void* = nullptr)
        {}
        
        void colors_palette_save_as_default(void* = nullptr)
        {}
        
        void colors_palette_new(void* = nullptr)
        {}
        
        void colors_palette_new_from_image(void* = nullptr)
        {}
        
        void colors_palette_edit(void* = nullptr)
        {}
        
        void colors_invert(void* = nullptr)
        {}
        
        void colors_grayscale(void* = nullptr)
        {}
        
        void colors_hue(void* = nullptr)
        {}
        
        void colors_saturation(void* = nullptr)
        {}
        
        void colors_value(void* = nullptr)
        {}
        
        void colors_replace(void* = nullptr)
        {}
        
        void colors_palette_shift(void* = nullptr)
        {}

        static inline MenuModel* _layers_frames_menu_model = nullptr;
        static inline MenuModel* _layers_menu_model = nullptr;
        
        void layers_new_layer(void* = nullptr)
        {}
        
        void layers_duplicate_layer(void* = nullptr)
        {}
        
        void layers_delete_layer(void* = nullptr)
        {}
        
        void layers_merge_down(void* = nullptr)
        {}
        
        void layers_flatten_all(void* = nullptr)
        {}
        
        void layers_hide_all(void* = nullptr)
        {}
        
        void layers_show_all(void* = nullptr)
        {}
        
        void layers_lock_all(void* = nullptr)
        {}
        
        void layers_unlock_all(void* = nullptr)
        {}

        static inline MenuModel* _frames_menu_model = nullptr;

        void frames_new_key_frame(void* = nullptr)
        {}
        
        void frames_new_inbetween_frame(void* = nullptr)
        {}
        
        void frames_duplicate(void* = nullptr)
        {}
        
        void frames_delete(void* = nullptr)
        {}

        static inline MenuModel* _view_menu_model = nullptr;

        void view_show_toolbar(void* = nullptr)
        {}
        
        void view_show_palette(void* = nullptr)
        {}
        
        void view_show_color_swapper(void* = nullptr)
        {}
        
        void view_show_color_picker(void* = nullptr)
        {}
        
        void view_show_verbose_color_picker(void* = nullptr)
        {}
        
        void view_show_brush_options(void* = nullptr)
        {}
        
        void view_show_animation_preview(void* = nullptr)
        {}
        
        void view_show_layer_view(void* = nullptr)
        {}

        static inline MenuModel* _settings_menu_model = nullptr;
        static inline MenuModel* _settings_keybinding_menu_model = nullptr;

        void settings_keybindings_show(void* = nullptr)
        {}
        
        void settings_keybindings_edit(void* = nullptr)
        {}

        static inline MenuModel* _settings_animation_menu_model = nullptr;

        void settings_animation_set_fps(void* = nullptr)
        {}
        
        void settings_animation_set_set_onion_skin(void* = nullptr)
        {}
        
        static inline MenuModel* _setings_canvas_menu_model = nullptr;
        
        void settings_canvas_set_show_grid(void* = nullptr)
        {}

        void settings_canvas_scroll_options(void* = nullptr)
        {}

        static inline MenuModel* _other_menu_model;

        void other_show_manual(void* = nullptr)
        {}

        void other_show_about_info(void* = nullptr)
        {}

        void other_github_link(void* = nullptr)
        {}

        void other_donate_link(void* = nullptr)
        {}
    }

    void setup_global_menu_bar_model()
    {
        state::global_menu_bar_model = new MenuModel();
        auto* app = state::app;
        
        using namespace MenuBar;


    }
}