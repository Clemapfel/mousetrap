//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), 1/6/23
//

#pragma once

#include <mousetrap.hpp>
#include <app/global_state.hpp>

namespace mousetrap
{
    KeyFile export_state_to_file()
    {
        KeyFile out;

        std::string current_section = "colors";

        auto set = [&]<typename T>(const std::string& key, T value) -> void {
            out.set_value_as<T>(current_section, key, value);
        };

        set("primary_color", state::primary_color);
        set("secondary_color", state::secondary_color);

        current_section = "palette";

        set("sort_mode", palette_sort_mode_to_string(state::palette_sort_mode));

        for (const auto& pair : state::palette.data())
            set(std::to_string(pair.first), pair.second);

        current_section = "tools";

        set("active_tool", state::active_tool);
        set("current_brush", state::current_brush->get_name());
        set("brush_size", state::brush_size);
        set("brush_opacity", state::brush_opacity);

        for (auto* brush : state::brushes)
        {

        }
    }
}
