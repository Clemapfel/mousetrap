// 
// Copyright 2022 Clemens Cords
// Created on 10/11/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <app/global_state.hpp>
#include <app/save_file.hpp>
#include <app/selection.hpp>
#include <app/image_transform.hpp>
#include <app/palette_view.hpp>
#include <app/layer_view.hpp>

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
        file_submenu_save_section.add_action("New...", save_file_new_state_action.get_id());
        file_submenu_save_section.add_action("Save As...", save_file_save_state_to_file_action.get_id());
        file_submenu_save_section.add_action("Save", "TODO");
        file_submenu_save_section.add_action("Open...", save_file_load_state_from_file_action.get_id());
        file_submenu.add_section("Load / Save", &file_submenu_save_section);

        auto file_submenu_import_section = MenuModel();
        file_submenu_import_section.add_action("Import...", save_file_import_from_image_action.get_id());
        file_submenu_import_section.add_action("Export As...", save_file_export_as_image_action.get_id());
        file_submenu_import_section.add_action("Export As Spritesheet...", save_file_export_as_spritesheet_action.get_id());
        file_submenu_import_section.add_action("Export Metadata...", save_file_export_metadata_action.get_id());
        file_submenu.add_section("Import / Export", &file_submenu_import_section);

        auto file_submenu_close_section = MenuModel();
        file_submenu_close_section.add_action("Exit", save_file_safe_exit_action.get_id());

        main.add_submenu("File", &file_submenu);

        // SELECTION

        auto selection_submenu = MenuModel();

        selection_submenu.add_action("Select All", selection_select_all_action.get_id());
        selection_submenu.add_action("Select Color...", selection_open_select_color_dialog_action.get_id());
        selection_submenu.add_action("Invert", selection_invert_action.get_id());

        main.add_submenu("Selection", &selection_submenu);

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

        main.add_submenu("Image", &image_submenu);

        // COLORS

        auto colors_submenu = MenuModel();

        auto color_submenu_palette_section = MenuModel();

        color_submenu_palette_section.add_action("Load...", palette_view_on_load_action.get_id());
        color_submenu_palette_section.add_action("Load Default", palette_view_on_load_default_action.get_id());
        color_submenu_palette_section.add_action("Save", palette_view_on_save_action.get_id());
        color_submenu_palette_section.add_action("Save As...", palette_view_on_save_as_action.get_id());
        color_submenu_palette_section.add_action("Save As Default", palette_view_on_save_as_default_action.get_id());

        colors_submenu.add_submenu("Palette", &color_submenu_palette_section);

        auto color_submenu_transform_section = MenuModel();
        color_submenu_transform_section.add_action("Invert", color_transform_invert_action.get_id());
        color_submenu_transform_section.add_action("To Grayscale", color_transform_to_grayscale_action.get_id());
        color_submenu_transform_section.add_action("Hue...", color_transform_open_hsv_shift_dialog_action.get_id());
        color_submenu_transform_section.add_action("Saturation...", color_transform_open_hsv_shift_dialog_action.get_id());
        color_submenu_transform_section.add_action("Value...", color_transform_open_hsv_shift_dialog_action.get_id());
        color_submenu_transform_section.add_action("Replace...", color_transform_open_color_replace_dialog_action.get_id());
        color_submenu_transform_section.add_action("Palette Shift...", color_transform_open_palette_shift_dialog_action.get_id());

        colors_submenu.add_section("Transform", &color_submenu_transform_section);

        main.add_submenu("Color", &colors_submenu);

        // LAYERS



        // OTHER

        auto other_submenu = MenuModel();

        other_submenu.add_action("Manual...", menu_open_manual_link_action.get_id());
        other_submenu.add_action("About...", menu_open_about_dialog_action.get_id());
        other_submenu.add_action("GitHub...", menu_open_github_link_action.get_id());
        other_submenu.add_action("Donate...", menu_open_donate_link_action.get_id());

        main.add_submenu("Other", &other_submenu);
    }
}