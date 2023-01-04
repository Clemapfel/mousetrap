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

        /*
        auto add_action = [&](MenuModel* model, const std::string frame_label, const std::string& action_id, auto f){
            app->add_action(action_id, f, (void*) nullptr);
            model->add_action(frame_label, "app." + action_id);
        };

        // FILE

        _file_menu_model = new MenuModel();
        _file_open_recent_menu_model = new MenuModel();
        _file_new_create_menu_model = new MenuModel();
        _file_save_export_menu_model = new MenuModel();

        state::global_menu_bar_model->add_submenu("File", _file_menu_model);
        _file_menu_model->add_section("Create", _file_new_create_menu_model);
        add_action(_file_new_create_menu_model, "New...", "file_new", file_new);
        add_action(_file_new_create_menu_model, "Open...", "file_open",file_open);
        _file_new_create_menu_model->add_submenu("Open Recent", _file_open_recent_menu_model);
        _file_menu_model->add_section("Export", _file_save_export_menu_model);
        add_action(_file_save_export_menu_model, "Save", "file_save", file_save);
        add_action(_file_save_export_menu_model, "Save As...", "file_save_as", file_save_as);
        add_action(_file_save_export_menu_model, "Export", "file_export", file_export);
        add_action(_file_save_export_menu_model, "Export As...", "file_export_as", file_export_as);
        add_action(_file_save_export_menu_model, "Export As Spritesheet...", "file_export_as_spritesheet", file_export_as_spritesheet);

        //add_action(_file_menu_model, "Close", "file_close", file_close);
        
        // SELECTION
        
        _selection_menu_model = new MenuModel();

        state::global_menu_bar_model->add_submenu("Selection", _selection_menu_model);
        add_action(_selection_menu_model, "Select All", "selection_select_all", selection_select_all);
        add_action(_selection_menu_model, "Invert", "selection_invert", selection_invert);
        add_action(_selection_menu_model, "Select By Color...", "selection_select_by_color", selection_select_by_color);

        // IMAGE

        _image_menu_model = new MenuModel();
        _image_size_menu_model = new MenuModel();
        _image_transform_menu_model = new MenuModel();

        state::global_menu_bar_model->add_submenu("Image", _image_menu_model);
        _image_menu_model->add_section("Size", _image_size_menu_model);
        add_action(_image_size_menu_model, "Resize Canvas...", "image_resize_canvas", image_resize_canvas);
        add_action(_image_size_menu_model, "Scale...", "image_crop_to_selection", image_crop_to_selection);
        add_action(_image_size_menu_model, "Crop to Selection", "image_scale_image", image_scale_image);

        _image_menu_model->add_section("Transform", _image_transform_menu_model);
        add_action(_image_transform_menu_model, "Flip Horizontally", "image_transform_flip_horizontally", image_transform_flip_horizontally);
        add_action(_image_transform_menu_model, "Flip Vertically", "image_transform_flip_vertically", image_transform_flip_vertically);
        add_action(_image_transform_menu_model, "Rotate Clockwise", "image_rotate_clockwise", image_transform_rotate_clockwise);
        add_action(_image_transform_menu_model, "Rotate Counter-clockwise", "image_rotate_clockwise", image_transform_rotate_clockwise);

        // COLORS

        _colors_menu_model = new MenuModel();
        _colors_palette_menu_model = new MenuModel();
        _colors_transform_menu_model = new MenuModel();

        state::global_menu_bar_model->add_submenu("Colors", _colors_menu_model);
        _colors_menu_model->add_section("Palette", _colors_palette_menu_model);
        add_action(_colors_palette_menu_model, "Load...", "colors_palette_load", colors_palette_load);
        add_action(_colors_palette_menu_model, "Load Default", "colors_palette_load_default", colors_palette_load_default);
        add_action(_colors_palette_menu_model, "Save...", "colors_palette_save", colors_palette_save);
        add_action(_colors_palette_menu_model, "Save As Default", "colors_palette_save_as_default", colors_palette_save_as_default);
        add_action(_colors_palette_menu_model, "New...", "colors_palette_new", colors_palette_new);
        add_action(_colors_palette_menu_model, "New From Image", "colors_palette_new_from_image", colors_palette_new_from_image);
        add_action(_colors_palette_menu_model, "Edit...", "colors_palette_edit", colors_palette_edit);

        _colors_menu_model->add_section("Transform", _colors_transform_menu_model);
        add_action(_colors_transform_menu_model, "Invert", "colors_invert", colors_invert);
        add_action(_colors_transform_menu_model, "Grayscale", "colors_grayscale", colors_grayscale);
        add_action(_colors_transform_menu_model, "Hue...", "colors_hue", colors_hue);
        add_action(_colors_transform_menu_model, "Saturation...", "colors_saturation", colors_saturation);
        add_action(_colors_transform_menu_model, "Value...", "colors_value", colors_value);
        add_action(_colors_transform_menu_model, "Replace...", "colors_replace", colors_replace);
        add_action(_colors_transform_menu_model, "Palette Shift...", "colors_palette_shift", colors_palette_shift);
        
        // LAYERS / FRAMES

        _layers_frames_menu_model = new MenuModel();
        _layers_menu_model = new MenuModel();
        _frames_menu_model = new MenuModel();

        state::global_menu_bar_model->add_submenu("Layers / Frames", _layers_frames_menu_model);
        _layers_frames_menu_model->add_section("Layers", _layers_menu_model);
        add_action(_layers_menu_model, "New Layer...", "layers_new_layer", layers_new_layer);
        add_action(_layers_menu_model, "Duplicate Layer", "layers_duplicate_layer", layers_duplicate_layer);
        add_action(_layers_menu_model, "Delete Layer", "layers_delete_layer", layers_delete_layer);
        add_action(_layers_menu_model, "Merge Down", "layers_merge_down", layers_merge_down);
        add_action(_layers_menu_model, "Flatten All", "layers_flatten_all", layers_flatten_all);
        
        _layers_frames_menu_model->add_section("Frames", _frames_menu_model);
        add_action(_frames_menu_model, "New Key Frame", "frames_new_key_frame", frames_new_key_frame);
        add_action(_frames_menu_model, "New Inbetween Frames", "frames_new_inbetween_frames", frames_new_inbetween_frame);
        add_action(_frames_menu_model, "Duplicate Frame", "frames_duplicate", frames_duplicate);
        add_action(_frames_menu_model, "Delete Frame", "frames_delete", frames_delete);

        // VIEW
        _view_menu_model = new MenuModel();
        auto* _view_menu_model_inner = new MenuModel();
        state::global_menu_bar_model->add_submenu("View", _view_menu_model);

        _view_menu_model->add_section("Show / Hide", _view_menu_model_inner);
        add_action(_view_menu_model_inner, "Toolbar", "view_show_toolbar", view_show_toolbar);
        add_action(_view_menu_model_inner, "Palette", "view_show_palette", view_show_palette);
        add_action(_view_menu_model_inner, "Color Swapper", "view_show_color_swapper", view_show_color_swapper);
        add_action(_view_menu_model_inner, "Color Picker", "view_show_verbose_color_picker", view_show_verbose_color_picker);
        add_action(_view_menu_model_inner, "HSV Color Select", "view_show_color_picker", view_show_color_picker);
        add_action(_view_menu_model_inner, "Brush Options", "view_show_brush_options", view_show_brush_options);
        add_action(_view_menu_model_inner, "Preview", "view_show_animation_preview", view_show_animation_preview);
        add_action(_view_menu_model_inner, "Layers / Frames", "view_show_layer_view", view_show_layer_view);

        // SETTINGS

        _settings_menu_model = new MenuModel();
        _settings_keybinding_menu_model = new MenuModel();
        _settings_animation_menu_model = new MenuModel();
        _setings_canvas_menu_model = new MenuModel();

        state::global_menu_bar_model->add_submenu("Settings", _settings_menu_model);
        _settings_menu_model->add_section("Canvas", _setings_canvas_menu_model);
        add_action(_setings_canvas_menu_model, "Grid...", "settings_canvas_set_show_grid", settings_canvas_set_show_grid);
        add_action(_setings_canvas_menu_model, "Scroll...", "settings_canvas_scroll_options", settings_canvas_scroll_options);
        _settings_menu_model->add_section("Animation", _settings_animation_menu_model);
        add_action(_settings_animation_menu_model, "FPS...", "settings_animation_set_fps", settings_animation_set_fps);
        add_action(_settings_animation_menu_model, "Onion Skin...", "settings_animation_set_set_onion_skin", settings_animation_set_set_onion_skin);
        _settings_menu_model->add_section("Keymap", _settings_keybinding_menu_model);
        add_action(_settings_keybinding_menu_model, "Show", "settings_keybindings_show", settings_keybindings_show);
        add_action(_settings_keybinding_menu_model, "Edit...", "settings_keybindings_edit", settings_keybindings_edit);

        // OTHER

        _other_menu_model = new MenuModel();
        state::global_menu_bar_model->add_submenu("Other", _other_menu_model);

        add_action(_other_menu_model, "Help...", "other_show_manual", other_show_manual);
        add_action(_other_menu_model, "About...", "other_show_about_info", other_show_about_info);
        add_action(_other_menu_model, "Contribute on GitHub", "other_github_link", other_github_link);
        add_action(_other_menu_model, "Donate", "other_donate_link", other_github_link);

         */
    }
}