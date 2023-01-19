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

        std::deque<Brush>& load_default_brushes();
        std::vector<HSVA> load_default_palette_colors();

        // ### global instances ##############################

        static inline Window* main_window = nullptr;
        static inline Application* app = nullptr;
        static inline MenuModel* global_menu_bar_model = nullptr;
        static inline ShortcutController* shortcut_controller = nullptr;

        static inline size_t margin_unit = 10;
    }

    class ProjectState
    {
        public:
            ProjectState(Vector2i layer_resolution);

            const Brush* get_current_brush() const;
            void set_current_brush(size_t);
            size_t get_n_brushes() const;

            const Layer* get_current_layer() const;
            size_t get_n_layers() const;

            const Layer::Frame* get_current_frame() const;
            size_t get_n_frames() const;

            void draw_to_layer(size_t layer_i, size_t frame_i, std::map<Vector2i, HSVA>);
            void set_current_layer_and_frame(size_t layer_i, size_t frame_i);

            HSVA get_primary_color() const;
            void set_primary_color(HSVA);

            HSVA get_secondary_color() const;
            void set_secondary_color(HSVA);

            const Palette& get_palette() const;
            void set_palette(const std::vector<HSVA>&);

            PaletteSortMode get_palette_sort_mode() const;
            void set_palette_sort_mode(PaletteSortMode);

            void set_selection(Vector2Set<int>);
            const Vector2Set<int>& get_selection() const;
            void select_all();

            void set_playback_active(bool);
            bool get_playback_active() const;

            void set_onionskin_visible(bool);
            bool get_onionskin_visible() const;

            void set_n_onionskin_layers(size_t);
            size_t get_n_onionskin_layers() const;

        private:
            HSVA _primary_color = RGBA(1, 0, 1, 1).operator HSVA();
            HSVA _secondary_color = RGBA(1, 1, 0, 1).operator HSVA();

            Palette _palette;
            PaletteSortMode _palette_sort_mode = PaletteSortMode::NONE;

            HSVA _preview_color_current;
            HSVA _preview_color_previous;

            ToolID _active_tool = BRUSH;

            std::deque<Brush> _brushes;
            size_t _current_brush_i = 0;
            size_t _brush_size = 1;
            float _brush_opacity = 1;

            Vector2Set<int> _selection;

            Vector2ui _layer_resolution;
            std::deque<Layer> _layers;

            size_t _current_layer_i = 0;
            size_t _current_frame_i = 0;
            size_t _n_frames = 0;

            bool _playback_active = false;
            bool _onionskin_visible = false;
            size_t _onionskin_n_layers = 0;
    };

    static inline std::vector<ProjectState*> project_states = {};
    static inline ProjectState* active_state = nullptr;
}
