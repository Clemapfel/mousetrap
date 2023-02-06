// 
// Copyright 2022 Clemens Cords
// Created on 8/15/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <map>
#include <vector>
#include <list>
#include <deque>

#include <mousetrap.hpp>

#include <app/layer.hpp>
#include <app/palette.hpp>
#include <app/brush.hpp>
#include <app/tools.hpp>

namespace mousetrap
{
    namespace state
    {
        using namespace mousetrap;

        std::deque<Brush> load_default_brushes(const std::string& path);
        std::vector<HSVA> load_default_palette_colors();

        inline Window* main_window = nullptr;
        inline Application* app = nullptr;
        inline MenuModel* global_menu_bar_model = nullptr;
        inline ShortcutController* shortcut_controller = nullptr;

        inline size_t margin_unit = 10;
    }

    using CellPosition = Vector2ui;

    class ProjectState
    {
        public:
            ProjectState(Vector2i layer_resolution);

            const Brush* get_current_brush() const;
            void set_current_brush(size_t);
            size_t get_current_brush_index() const;
            size_t get_n_brushes() const;
            const Brush* get_brush(size_t) const;
            const std::deque<Brush>& get_brushes() const;

            void remove_brush(size_t);
            void add_brush(Brush);
            void load_default_brushes();
            static std::string get_default_brush_directory();

            float get_brush_opacity() const;
            void set_brush_opacity(float);

            size_t get_brush_size() const;
            void set_brush_size(size_t);

            ToolID get_active_tool() const;
            void set_active_tool(ToolID);

            const Layer* get_current_layer() const;
            size_t get_current_layer_index() const;
            const Layer* get_layer(size_t) const;
            size_t get_n_layers() const;

            const Layer::Frame* get_current_frame() const;
            size_t get_current_frame_index() const;
            const Layer::Frame* get_frame(size_t layer_i, size_t frame_i) const;
            size_t get_n_frames() const;

            void set_current_layer_and_frame(size_t layer_i, size_t frame_i);

            void add_layer(int above); //-1 for new layer at 0
            void duplicate_layer(int create_above, size_t duplicate_from);
            void swap_layers(size_t a, size_t b);
            void delete_layer(size_t);
            void new_layer_from(int above, const std::set<size_t>& from_layer_is, bool delete_froms = true);

            void add_frame(int after, bool is_keyframe = true); // -1 for left-most
            void swap_frames(size_t a, size_t b);
            void duplicate_frame(int after, size_t from_frame_i);
            void delete_frame(size_t);
            void new_frame_from(int after, const std::set<size_t>& from_layer_is, bool delete_froms = true);

            void copy_to_cell(CellPosition b, CellPosition a);
            void swap_cells(CellPosition a, CellPosition b);
            void draw_to_cell(CellPosition, std::vector<std::pair<Vector2i, HSVA>>);
            void set_cell_is_key(CellPosition, bool);

            void set_layer_blend_mode(size_t, BlendMode);
            void set_layer_locked(size_t, bool);
            void set_layer_visible(size_t, bool);
            void set_layer_opacity(size_t, float);
            void set_layer_name(size_t, const std::string&);

            Vector2ui get_layer_resolution() const;
            void resize_canvas(Vector2ui new_size, Vector2i offset);
            void scale_canvas(Vector2ui, GdkInterpType = GDK_INTERP_TILES);

            void flip_horizontally(size_t layer_i, size_t frame_i);
            void flip_vertically(size_t layer_i, size_t frame_i);
            void rotate_clockwise(size_t layer_i, size_t frame_i);
            void rotate_counterclockwise(size_t layer_i, size_t frame_i);

            HSVA get_primary_color() const;
            void set_primary_color(HSVA);

            HSVA get_secondary_color() const;
            void set_secondary_color(HSVA);

            void set_primary_and_secondary_color(HSVA primary, HSVA secondary);
            void set_color_component_offset(float h, float s, float v, float r, float g, float b, float a);

            HSVA get_preview_color_current() const;
            HSVA get_preview_color_previous() const;
            void set_preview_colors(HSVA current, HSVA previous);

            const Palette& get_palette() const;
            void set_palette(const std::vector<HSVA>&);

            const Palette& get_default_palette() const;
            void set_default_palette(const std::vector<HSVA>&);

            PaletteSortMode get_palette_sort_mode() const;
            void set_palette_sort_mode(PaletteSortMode);

            bool get_palette_editing_enabled() const;
            void set_palette_editing_enabled(bool b);

            void set_selection(Vector2Set<int>);
            const Vector2Set<int>& get_selection() const;
            void select_all();

            void set_playback_active(bool);
            bool get_playback_active() const;

            void set_onionskin_visible(bool);
            bool get_onionskin_visible() const;

            void set_n_onionskin_layers(size_t);
            size_t get_n_onionskin_layers() const;

            void set_fps(float);
            float get_fps() const;

        private:
            HSVA _primary_color = RGBA(1, 0, 1, 1).operator HSVA();
            HSVA _secondary_color = RGBA(1, 1, 0, 1).operator HSVA();

            Palette _palette;
            PaletteSortMode _palette_sort_mode = PaletteSortMode::NONE;
            bool _palette_editing_enabled = false;

            mutable Palette _default_palette;
            std::string _default_palette_path = get_resource_path() + "default.palette";

            HSVA _preview_color_current;
            HSVA _preview_color_previous;

            ToolID _active_tool = BRUSH;

            std::deque<Brush> _brushes;
            size_t _current_brush_i = 0;
            size_t _brush_size = 1;
            float _brush_opacity = 1;

            Vector2Set<int> _selection;

            Vector2ui _layer_resolution;
            std::deque<Layer*> _layers;

            size_t _current_layer_i = 0;
            size_t _current_frame_i = 0;
            size_t _n_frames = 0;

            bool _playback_active = false;
            float _playback_fps = 12.5;

            bool _onionskin_visible = false;
            size_t _onionskin_n_layers = 0;

            static inline size_t new_layer_count = 0;
            static inline size_t merged_layer_count = 0;

            void signal_brush_selection_changed();
            void signal_brush_set_updated();
            void signal_color_selection_changed();
            void signal_palette_updated();
            void signal_palette_sort_mode_changed();
            void signal_palette_editing_toggled();
            void signal_selection_changed();
            void signal_onionskin_visibility_toggled();
            void signal_onionskin_layer_count_changed();
            void signal_layer_frame_selection_changed();
            void signal_layer_image_updated();
            void signal_layer_count_changed();
            void signal_layer_properties_changed();
            void signal_active_tool_changed();
            void signal_layer_resolution_changed();
            void signal_playback_toggled();
            void signal_playback_fps_changed();
    };

    inline std::vector<ProjectState*> project_states = {};
    inline ProjectState* active_state = nullptr;
}
