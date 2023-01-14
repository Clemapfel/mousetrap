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
    namespace state::actions
    {
        Action save_file_save_state_to_file_action = Action("save_file.export_state_to_file");
        Action save_file_load_state_from_file_action = Action("save_file.load_state_from_file");

        Action save_file_export_as_image = Action("save_file.export_as_image");
        Action save_file_export_as_spritesheet = Action("save_file.export_as_spritesheet");
        Action save_file_export_metadata = Action("save_file.export_as_metadata");

        Action save_file_import_from_image = Action("save_file.import_from_image");
    }

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
        set("n_colors", state::palette.get_colors().size());

        for (const auto& pair : state::palette.data())
            set("color_" + std::to_string(pair.first), pair.second);

        current_section = "tools";

        set("active_tool", state::active_tool);

        current_section = "brushes";

        std::cerr << "In export_state_to_file: TODO" << std::endl;

        /*

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

         */

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

            prefix += "_frame";
            for (size_t frame_i = 0; frame_i < state::n_frames; ++frame_i)
            {
                set(prefix + std::to_string(frame_i) + "_is_keyframe", layer->frames.at(frame_i).is_keyframe);
                set(prefix + std::to_string(frame_i) + "_data", *layer->frames.at(frame_i).image);
            }
        }

        return out;
    }

    void import_state_from_file(KeyFile& in)
    {
        // TODO backup before load

        std::string current_section = "colors";

        state::primary_color = in.get_value_as<HSVA>(current_section, "primary_color");
        state::secondary_color = in.get_value_as<HSVA>(current_section, "secondary_color");
        state::preview_color_current = state::primary_color;
        state::preview_color_previous = state::primary_color;

        state::update_color_picker();
        state::update_color_swapper();
        state::update_verbose_color_picker();

        current_section = "palette";

        state::palette_sort_mode = palette_sort_mode_from_string(in.get_value(current_section, "sort_mode"));

        {
            size_t n = in.get_value_as<size_t>(current_section, "n_colors");
            std::vector<HSVA> colors;
            colors.reserve(n);

            for (size_t i = 0; i < n; ++i)
                colors.push_back(in.get_value_as<HSVA>(current_section, "color_" + std::to_string(i)));

            state::palette = Palette(colors);
        }

        current_section = "palette_view";

        auto* palette_view = ((PaletteView*) state::palette_view);
        palette_view->reload();
        palette_view->set_preview_size(in.get_value_as<size_t>(current_section, "preview_size"));
        palette_view->set_palette_locked(in.get_value_as<size_t>(current_section, "palette_locked"));

        current_section = "tools";

        state::active_tool = in.get_value_as<std::string>(current_section, "active_tool");
        ((Toolbox*) state::toolbox)->select(state::active_tool);

        current_section = "brushes";

        std::cerr << "In import_state_from_file: TODO" << std::endl;

        /*

        state::brush_size = in.get_value_as<size_t>(current_section, "brush_size");
        state::brush_opacity = in.get_value_as<float>(current_section, "brush_opacity");

        for (auto* brush : state::brushes)
            delete brush;

        state::brushes.clear();
        auto current_brush_name = in.get_value_as<std::string>(current_section, "current_brush");

        {
            size_t n = in.get_value_as<size_t>(current_section, "n_brushes");
            state::brushes.reserve(n);

            for (size_t i = 0; i < n; ++i)
            {
                auto prefix = "brush_" + std::to_string(i);
                auto name = in.get_value_as<std::string>(current_section, prefix + "_name");
                auto image = in.get_value_as<Image>(current_section, prefix + "_data");

                auto* brush = state::brushes.emplace_back(new Brush());
                brush->as_custom(image);

                if (name == current_brush_name)
                    state::current_brush = brush;
            }
        }

         */

        current_section = "animation_preview";

        auto* animation_preview = (AnimationPreview*) state::animation_preview;
        animation_preview->set_scale_factor(in.get_value_as<size_t>(current_section, "scale_factor"));
        animation_preview->set_fps(in.get_value_as<float>(current_section, "fps"));
        animation_preview->set_background_visible(in.get_value_as<bool>(current_section, "background_visible"));

        state::onionskin_visible = in.get_value_as<bool>(current_section, "onionskin_visible");
        state::onionskin_n_layers = in.get_value_as<size_t>(current_section, "onionskin_n_layers");

        current_section = "brush_options";
        ((BrushOptions*) state::brush_options)->set_preview_size(in.get_value_as<size_t>(current_section, "preview_size"));

        current_section = "layer_view";
        ((LayerView*) state::layer_view)->set_preview_size(in.get_value_as<size_t>(current_section, "preview_size"));

        current_section = "frame_view";
        ((FrameView*) state::frame_view)->set_preview_size(in.get_value_as<size_t>(current_section, "preview_size"));

        current_section = "layers";
        state::current_layer = in.get_value_as<size_t>(current_section, "current_layer");
        state::current_frame = in.get_value_as<size_t>(current_section, "current_frame");

        size_t n_layers = in.get_value_as<size_t>(current_section, "n_layers");
        size_t n_frames = in.get_value_as<size_t>(current_section, "n_frames");
        state::n_frames = n_frames;

        for (auto* layer : state::layers)
            delete layer;

        state::layers.clear();

        for (size_t layer_i = 0; layer_i < n_layers; ++layer_i)
        {
            auto prefix = "layer_" + std::to_string(layer_i);
            auto* layer = state::layers.emplace_back(new Layer());

            layer->name = in.get_value_as<std::string>(current_section, prefix + "_name");
            layer->is_locked = in.get_value_as<bool>(current_section, prefix + "_is_locked");
            layer->is_visible = in.get_value_as<bool>(current_section, prefix + "_is_visible");
            layer->blend_mode = blend_mode_from_string(in.get_value_as<std::string>(current_section, prefix + "_blend_mode"));
            layer->opacity = in.get_value_as<float>(current_section, prefix + "_opacity");

            prefix += "_frame";
            for (size_t frame_i = 0; frame_i < n_frames; ++frame_i)
            {
                auto& frame = layer->frames.emplace_back();
                frame.is_keyframe = in.get_value_as<bool>(current_section, prefix + "_is_keyframe");
                frame.image = new Image(in.get_value_as<Image>(current_section, prefix + "_data"));
                frame.texture = new Texture();
                frame.texture->create_from_image(*frame.image);
            }
        }

        state::update_palette_view();
    }
}
