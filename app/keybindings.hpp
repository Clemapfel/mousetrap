// 
// Copyright 2022 Clemens Cords
// Created on 8/15/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <gtk/gtk.h>

#include <map>
#include <string>
#include <iostream>

namespace mousetrap
{
    using GdkKey = size_t;

    namespace state
    {
        std::map<std::string, GdkKey> keybindings = {

            // palette view

            {"palette_view_increase_tile_size", GDK_KEY_plus},
            {"palette_view_decrease_tile_size", GDK_KEY_minus},
            {"palette_view_select_1",           GDK_KEY_1},
            {"palette_view_select_2",           GDK_KEY_2},
            {"palette_view_select_3",           GDK_KEY_3},
            {"palette_view_select_4",           GDK_KEY_4},
            {"palette_view_select_5",           GDK_KEY_5},
            {"palette_view_select_6",           GDK_KEY_6},
            {"palette_view_select_7",           GDK_KEY_7},
            {"palette_view_select_8",           GDK_KEY_8},
            {"palette_view_select_9",           GDK_KEY_9},
        };
    }

    GdkKey get_keybinding(const std::string& id)
    {
        auto it = state::keybindings.find(id);

        if (it == state::keybindings.end())
        {
            std::cerr << "[ERROR] In get_keybindings: no keybinding for \"" << id << "\" exists." << std::endl;
            return size_t(-1);
        }

        return it->second;
    }
}