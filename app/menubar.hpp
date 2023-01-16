// 
// Copyright 2022 Clemens Cords
// Created on 10/11/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <app/global_state.hpp>
#include <app/save_file.hpp>
#include <app/selection.hpp>
#include <app/image_transform.hpp>
#include <app/color_transform.hpp>
#include <app/palette_view.hpp>
#include <app/layer_view.hpp>
#include <app/frame_view.hpp>
#include <app/canvas.hpp>
#include <app/brush_options.hpp>

#include <include/menu_model.hpp>
#include <include/application.hpp>

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
namespace mousetrap
{
    namespace state::actions
    {
        Action menu_open_manual_link_action = Action("menu.open_manual_link");
        Action menu_open_about_dialog_action = Action("menu.open_about_dialog");
        Action menu_open_github_link_action = Action("menu.open_github_link");
        Action menu_open_donate_link_action = Action("menu.open_donate_link");

        Action menu_open_cursor_settings_action = Action("menu.open_cursor_settings");
        Action menu_open_keybinding_settings_action = Action("menu.open_keybinding_settings");
        Action menu_open_animation_settings_action = Action("menu.open_animation_settings");
        Action menu_open_color_settings_action = Action("menu.open_color_settings");
        Action menu_open_backup_settings_action = Action("menu.open_backup_settings");
        Action menu_open_settings_ini_file_action = Action("menu.open_settings_ini_file");
        Action menu_open_log_folder_action = Action("menu.open_log_folder");
        Action state_undo_action = Action("state.undo");
        Action state_redo_action = Action("state.redo");
    }

    void initialize_menubar_actions()
    {
        static auto open_uri = [](const std::string& uri)
        {
            GError* error = nullptr;
            g_app_info_launch_default_for_uri(uri.c_str(), nullptr, &error);
            if (error != nullptr)
            {
                auto error_message = "Unable to open uri at `" + uri + ": " + error->message;
                ((BubbleLogArea*) state::bubble_log)->send_message(error_message);
                std::cerr << error_message << std::endl;
            }
        };
    }

    void setup_global_menu_bar_model()
    {
        state::global_menu_bar_model = new MenuModel();
        auto& main = *state::global_menu_bar_model;
        auto* app = state::app;

        using namespace state::actions;

        // FILE
        
        auto file_submenu = MenuModel();

        auto file_submenu_save_section = MenuModel();
        file_submenu_save_section.add_action("Open...", save_file_open_load_dialog_action.get_id());
        file_submenu_save_section.add_action("New", save_file_new_state_action.get_id());
        file_submenu_save_section.add_action("Save", save_file_save_state_to_file_action.get_id());
        file_submenu_save_section.add_action("Save As...", save_file_open_save_dialog_action.get_id());
        file_submenu_save_section.add_action("Restore from Backup...", save_file_open_restore_from_backup_dialog_action.get_id());
        file_submenu.add_section("Load / Save", &file_submenu_save_section);

        auto file_submenu_import_section = MenuModel();
        file_submenu_import_section.add_action("Import...", save_file_import_from_image_action.get_id());
        file_submenu_import_section.add_action("Export As...", save_file_export_as_image_action.get_id());
        file_submenu_import_section.add_action("Export As Spritesheet...", save_file_export_as_spritesheet_action.get_id());
        file_submenu_import_section.add_action("Export Spritesheet Metadata...", save_file_export_metadata_action.get_id());
        file_submenu.add_section("Import / Export", &file_submenu_import_section);

        auto file_submenu_close_section = MenuModel();
        file_submenu_close_section.add_action("Exit", save_file_safe_exit_action.get_id());
        
        // SELECTION

        auto selection_submenu = MenuModel();

        selection_submenu.add_action("Invert", selection_invert_action.get_id());
        selection_submenu.add_action("Select All", selection_select_all_action.get_id());
        selection_submenu.add_action("Select Color...", selection_open_select_color_dialog_action.get_id());

        // IMAGE

        auto image_submenu = MenuModel();

        auto image_submenu_size_section = MenuModel();
        image_submenu_size_section.add_action("Resize Canvas...", image_transform_open_resize_canvas_dialog_action.get_id());
        image_submenu_size_section.add_action("Scale Canvas...", image_transform_open_scale_canvas_dialog_action.get_id());
        image_submenu.add_section("Canvas Size", &image_submenu_size_section);

        auto image_submenu_transform_section = MenuModel();
        image_submenu_transform_section.add_action("Flip Horizontally", image_transform_flip_horizontally_action.get_id());
        image_submenu_transform_section.add_action("Flip Vertically", image_transform_flip_vertically_action.get_id());
        image_submenu_transform_section.add_action("Rotate +90°", image_transform_rotate_clockwise_action.get_id());
        image_submenu_transform_section.add_action("Rotate -90°", image_transform_rotate_counterclockwise_action.get_id());
        image_submenu.add_section("Transform", &image_submenu_transform_section);

        // COLORS

        auto colors_submenu = MenuModel();

        auto color_submenu_transform_section = MenuModel();
        color_submenu_transform_section.add_action("Invert", color_transform_invert_action.get_id());
        color_submenu_transform_section.add_action("To Grayscale", color_transform_to_grayscale_action.get_id());
        color_submenu_transform_section.add_action("Hue...", color_transform_open_hsv_shift_dialog_action.get_id());
        color_submenu_transform_section.add_action("Saturation...", color_transform_open_hsv_shift_dialog_action.get_id());
        color_submenu_transform_section.add_action("Value...", color_transform_open_hsv_shift_dialog_action.get_id());
        color_submenu_transform_section.add_action("Replace...", color_transform_open_color_replace_dialog_action.get_id());
        color_submenu_transform_section.add_action("Palette Shift...", color_transform_open_palette_shift_dialog_action.get_id());
        colors_submenu.add_section("Transform", &color_submenu_transform_section);

        auto color_palette_section = MenuModel();
        color_palette_section.add_action("Load...", palette_view_on_load_action.get_id());
        color_palette_section.add_action("Load Default", palette_view_on_load_default_action.get_id());
        color_palette_section.add_action("Save", palette_view_on_save_action.get_id());
        color_palette_section.add_action("Save As...", palette_view_on_save_as_action.get_id());
        color_palette_section.add_action("Save As Default", palette_view_on_save_as_default_action.get_id());
        colors_submenu.add_section("Palette", &color_palette_section);

        // LAYERS

        auto layer_submenu = MenuModel();

        auto layer_submenu_create_section = MenuModel();
        layer_submenu_create_section.add_action("New Layer", layer_view_layer_new_above_current_action.get_id());
        layer_submenu_create_section.add_action("New Layer Below", layer_view_layer_new_below_current_action.get_id());
        layer_submenu_create_section.add_action("New Layer From Visible", layer_view_layer_create_from_visible.get_id());
        layer_submenu_create_section.add_action("Duplicate Layer", layer_view_layer_duplicate_action.get_id());
        layer_submenu_create_section.add_action("Delete Layer", layer_view_layer_delete_action.get_id());
        layer_submenu.add_section("Create / Delete", &layer_submenu_create_section);

        auto layer_submenu_merge_section = MenuModel();
        layer_submenu_merge_section.add_action("Merge Down", layer_view_layer_merge_down_action.get_id());
        layer_submenu_merge_section.add_action("Flatten All", layer_view_layer_flatten_all_action.get_id());
        layer_submenu.add_section("Merge", &layer_submenu_merge_section);

        auto layer_move_section = MenuModel();
        layer_move_section.add_action("Move Layer Down", layer_view_layer_move_down_action.get_id());
        layer_move_section.add_action("Move Layer Up", layer_view_layer_move_up_action.get_id());
        layer_submenu.add_section("Layer Position", &layer_move_section);

        auto layer_show_section = MenuModel();
        layer_show_section.add_action("Hide / Show Current Layer", layer_view_set_layer_visible_action.get_id());
        layer_show_section.add_action("Hide All Other Layers", layer_view_hide_all_other_layers.get_id());
        layer_show_section.add_action("Show All Other Layers", layer_view_show_all_other_layers.get_id());
        layer_submenu.add_section("Hide / Show", &layer_show_section);

        // FRAMES

        auto frame_submenu = MenuModel();

        auto frame_create_section = MenuModel();
        frame_create_section.add_action("New Frame", frame_view_frame_new_right_of_current_action.get_id());
        frame_create_section.add_action("New Frame Before", frame_view_frame_new_left_of_current_action.get_id());
        frame_create_section.add_action("Delete Frame", frame_view_frame_delete_action.get_id());
        frame_submenu.add_section("Create / Delete", &frame_create_section);

        auto frame_move_section = MenuModel();
        frame_move_section.add_action("Move Frame Left", frame_view_frame_move_left_action.get_id());
        frame_move_section.add_action("Move Frame Right", frame_view_frame_move_right_action.get_id());
        frame_submenu.add_section("Frame Position", &frame_move_section);

        auto frame_navigation_section = MenuModel();
        frame_navigation_section.add_action("Play / Pause Playback", frame_view_play_pause_action.get_id());
        frame_navigation_section.add_action("Select Previous", frame_view_go_to_previous_frame_action.get_id());
        frame_navigation_section.add_action("Select Next", frame_view_go_to_next_frame_action.get_id());
        frame_navigation_section.add_action("Jump to Start", frame_view_jump_to_start_action.get_id());
        frame_navigation_section.add_action("Jump to End", frame_view_jump_to_end_action.get_id());
        frame_submenu.add_section("Playback", &frame_navigation_section);

        auto frame_other_section = MenuModel();
        frame_other_section.add_action("Toggle Keyframe / Inbetween", frame_view_frame_make_keyframe_inbetween_action.get_id());
        frame_other_section.add_action("Toggle Onionskin Visible", frame_view_toggle_onionskin_visible_action.get_id());
        frame_submenu.add_section("Other", &frame_other_section);

        // CANVAS
        
        auto canvas_submenu = MenuModel();

        auto canvas_grid_section = MenuModel();
        canvas_grid_section.add_action("Hide / Show Grid", canvas_toggle_grid_visible_action.get_id());
        canvas_grid_section.add_action("Grid Color...", canvas_open_grid_color_settings_action.get_id());
        canvas_submenu.add_section("Grid", &canvas_grid_section);

        auto canvas_brush_section = MenuModel();
        canvas_brush_section.add_action("Hide / Show Brush Outline", canvas_toggle_brush_outline_visible_action.get_id());
        canvas_submenu.add_section("Brush", &canvas_brush_section);

        auto canvas_mirror_section = MenuModel();
        canvas_mirror_section.add_action("Enable / Disable Horizontal Symmetry", canvas_toggle_brush_outline_visible_action.get_id());
        canvas_mirror_section.add_action("Enable / Disable Vertical Symmetry", canvas_toggle_brush_outline_visible_action.get_id());
        canvas_submenu.add_section("Symmetry", &canvas_mirror_section);

        auto canvas_perspective_section = MenuModel();
        canvas_perspective_section.add_action("Reset View", canvas_reset_transform_action.get_id());
        canvas_submenu.add_section("View", &canvas_perspective_section);

        // BRUSHES

        auto brushes_submenu = MenuModel();
        auto brush_add_section = MenuModel();
        brush_add_section.add_action("Add Brush...", brush_options_add_brush_action.get_id());
        brush_add_section.add_action("Remove Current Brush", brush_options_remove_brush_action.get_id());
        brushes_submenu.add_section("Add / Remove", &brush_add_section);

        auto brush_default_section = MenuModel();
        brush_default_section.add_action("Load Default Brushes", brush_options_load_default_brushes.get_id());
        brush_default_section.add_action("Manage Default Brushes...", brush_options_open_default_brush_directory_action.get_id());
        brushes_submenu.add_section("Defaults", &brush_default_section);

        // SETTINGS

        auto settings_submenu = MenuModel();

        auto settings_other_section = MenuModel();
        settings_other_section.add_action("Animation...", menu_open_animation_settings_action.get_id());
        settings_other_section.add_action("Colors...", menu_open_color_settings_action.get_id());
        settings_submenu.add_section("Project", &settings_other_section);

        auto settings_input_section = MenuModel();
        settings_input_section.add_action("Mouse...", menu_open_cursor_settings_action.get_id());
        settings_input_section.add_action("Tablet...", menu_open_cursor_settings_action.get_id());
        settings_input_section.add_action("Keybindings...", menu_open_keybinding_settings_action.get_id());
        settings_submenu.add_section("User Input", &settings_input_section);

        auto settings_system_section = MenuModel();
        settings_system_section.add_action("Backups...", menu_open_backup_settings_action.get_id());
        settings_system_section.add_action("Developer Settings...", menu_open_settings_ini_file_action.get_id());
        settings_submenu.add_section("System", &settings_system_section);

        // OTHER

        auto other_submenu = MenuModel();

        auto undo_section = MenuModel();
        undo_section.add_action("Undo", state_undo_action.get_id());
        undo_section.add_action("Redo", state_redo_action.get_id());
        other_submenu.add_section("Edit History", &undo_section);

        auto online_section = MenuModel();
        online_section.add_action("Manual...", menu_open_manual_link_action.get_id());
        online_section.add_action("GitHub...", menu_open_github_link_action.get_id());
        online_section.add_action("Donate...", menu_open_donate_link_action.get_id());
        other_submenu.add_section("Online", &online_section);

        auto troubleshooting_section = MenuModel();
        troubleshooting_section.add_action("About...", menu_open_about_dialog_action.get_id());
        troubleshooting_section.add_action("Logs...", menu_open_log_folder_action.get_id());
        other_submenu.add_section("Troubleshooting", &troubleshooting_section);

        // main layout

        main.add_submenu("File", &file_submenu);
        main.add_submenu("Image", &image_submenu);
        main.add_submenu("Selection", &selection_submenu);
        main.add_submenu("Colors", &colors_submenu);
        main.add_submenu("Canvas", &canvas_submenu);
        main.add_submenu("Layers", &layer_submenu);
        main.add_submenu("Frames", &frame_submenu);
        main.add_submenu("Brushes", &brushes_submenu);
        main.add_submenu("Settings", &settings_submenu);
        main.add_submenu("Other", &other_submenu);
    }
}