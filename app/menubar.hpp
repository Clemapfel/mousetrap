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

namespace mousetrap
{
    namespace state::actions
    {
        inline Action menu_open_manual_link_action = Action("menu.open_manual_link");
        inline Action menu_open_about_dialog_action = Action("menu.open_about_dialog");
        inline Action menu_open_github_link_action = Action("menu.open_github_link");
        inline Action menu_open_donate_link_action = Action("menu.open_donate_link");

        inline Action menu_open_cursor_settings_action = Action("menu.open_cursor_settings");
        inline Action menu_open_keybinding_settings_action = Action("menu.open_keybinding_settings");
        Action menu_open_animation_settings_action = Action("menu.open_animation_settings");
        Action menu_open_color_settings_action = Action("menu.open_color_settings");
        Action menu_open_backup_settings_action = Action("menu.open_backup_settings");
        Action menu_open_settings_ini_file_action = Action("menu.open_settings_ini_file");
        Action menu_open_log_folder_action = Action("menu.open_log_folder");
        Action state_undo_action = Action("state.undo");
        Action state_redo_action = Action("state.redo");
    }

    void initialize_menubar_actions();
    void setup_global_menu_bar_model();
}