// 
// Copyright 2022 Clemens Cords
// Created on 8/15/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <gtk/gtk.h>

#include <map>
#include <string>
#include <iostream>

using GdkKey = size_t;

namespace mousetrap
{
    namespace state
    {
        std::map<std::string, GdkKey> keybindings = {

            // palette view

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

    std::string key_to_string(GdkKey key)
    {
        switch (key)
        {
            case GDK_KEY_plus:
                return "+";
            case GDK_KEY_minus:
                return "-";
            case GDK_KEY_0:
                return "0";
            case GDK_KEY_1:
                return "1";
            case GDK_KEY_2:
                return "2";
            case GDK_KEY_3:
                return "3";
            case GDK_KEY_4:
                return "4";
            case GDK_KEY_5:
                return "5";
            case GDK_KEY_6:
                return "6";
            case GDK_KEY_7:
                return "7";
            case GDK_KEY_8:
                return "8";
            case GDK_KEY_9:
                return "9";

            default:
                std::cerr << "[ERROR] In key_to_string: key with code \"" << std::dec << key << "\" no recognized." << std::endl;
                return "";
        }
    }
}