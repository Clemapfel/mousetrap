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
        DECLARE_GLOBAL_ACTION(menu, open_manual_link);
        DECLARE_GLOBAL_ACTION(menu, open_about_dialog);
        DECLARE_GLOBAL_ACTION(menu, open_github_link);
        DECLARE_GLOBAL_ACTION(menu, open_donate_link);

        DECLARE_GLOBAL_ACTION(menu, open_cursor_settings);
        DECLARE_GLOBAL_ACTION(menu, open_keybinding_settings);
        DECLARE_GLOBAL_ACTION(menu, open_animation_settings);
        DECLARE_GLOBAL_ACTION(menu, open_color_settings);
        DECLARE_GLOBAL_ACTION(menu, open_backup_settings);
        DECLARE_GLOBAL_ACTION(menu, open_settings_ini_file);
        DECLARE_GLOBAL_ACTION(menu, open_log_folder);

        DECLARE_GLOBAL_ACTION(state, undo);
        DECLARE_GLOBAL_ACTION(state, redo);
    }

    void initialize_menubar_actions();
    void setup_global_menu_bar_model();
}