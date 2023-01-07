//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), 1/6/23
//

#pragma once

#include <mousetrap.hpp>
#include <app/global_state.hpp>
#include <app/animation_preview.hpp>
#include <app/brush_options.hpp>
#include <app/frame_view.hpp>
#include <app/layer_view.hpp>
#include <app/palette_view.hpp>

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
            set("color_" + std::to_string(pair.first), pair.second);

        current_section = "tools";

        set("active_tool", state::active_tool);

        current_section = "brushes";

        set("current_brush", state::current_brush->get_name());
        set("brush_size", state::brush_size);
        set("brush_opacity", state::brush_opacity);
        set("n_brushes", state::brushes.size());

        for (size_t i = 0; i < state::brushes.size(); ++i)
        {
            auto* brush = state::brushes.at(i);
            set("brush_" + std::to_string(i) + "_name", brush->get_name());
            set("brush_" + std::to_string(i) + "_data", brush->get_base_image());
        }

        current_section = "animation_preview";

        auto* animation_preview = (AnimationPreview*) state::animation_preview;
        set("scale_factor", animation_preview->get_scale_factor());
        set("fps", animation_preview->get_fps());
        set("background_visible", animation_preview->get_background_visible());
        set("onionskin_visible", state::onionskin_visible);
        set("onionskin_n_layers", state::onionskin_n_layers);

        current_section = "brush_options";

        set("preview_size", ((BrushOptions*) state::brush_options)->get_preview_size());

        current_section = "frame_view";

        set("preview_size", ((FrameView*) state::frame_view)->get_preview_size());

        current_section = "layer_view";

        set("preview_size", ((LayerView*) state::layer_view)->get_preview_size());

        current_section = "palette_view";

        set("preview_size", ((PaletteView*) state::palette_view)->get_preview_size());
        set("palette_locked", ((PaletteView*) state::palette_view)->get_palette_locked());

        current_section = "layers";

        set("current_layer", state::current_layer);
        set("current_frame", state::current_frame);

        set("n_layers", state::layers.size());
        set("n_frames", state::n_frames);

        for (size_t layer_i = 0; layer_i < state::layers.size(); ++layer_i)
        {
            auto* layer = state::layers.at(layer_i);
            auto prefix = "layer_" + std::to_string(layer_i);

            set(prefix + "_name", layer->name);
            set(prefix + "_is_locked", layer->is_locked);
            set(prefix + "_is_visible", layer->is_visible);
            set(prefix + "_blend_mode", blend_mode_to_string(layer->blend_mode));
            set(prefix + "_opacity", layer->opacity);

            prefix += "_frame_";
            for (size_t frame_i = 0; frame_i < state::n_frames; ++frame_i)
            {
                set(prefix + std::to_string(frame_i) + "_is_keyframe", layer->frames.at(frame_i).is_keyframe);
                set(prefix + std::to_string(frame_i) + "_data", *layer->frames.at(frame_i).image);
            }
        }

        return out;
    }

    void import_state_from_file(KeyFile& file)
    {}
}
