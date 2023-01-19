#include <app/global_state.hpp>
#include <app/brush_options.hpp>
#include <app/animation_preview.hpp>
#include <app/brush_options.hpp>
#include <app/bubble_log_area.hpp>
#include <app/canvas.hpp>
#include <app/color_picker.hpp>
#include <app/color_preview.hpp>
#include <app/color_swapper.hpp>
#include <app/color_transform.hpp>
#include <app/config_files.hpp>
#include <app/frame_view.hpp>
#include <app/global_state.hpp>
#include <app/image_transform.hpp>
#include <app/layer.hpp>
#include <app/layer_view.hpp>
#include <app/menubar.hpp>
#include <app/palette.hpp>
#include <app/palette_view.hpp>
#include <app/save_file.hpp>
#include <app/selection.hpp>
#include <app/shortcut_information.hpp>
#include <app/toolbox.hpp>
#include <app/tools.hpp>
#include <app/tooltip.hpp>
#include <app/verbose_color_picker.hpp>

namespace mousetrap
{
    std::deque<Brush>& state::load_default_brushes()
    {
        auto out = std::deque<Brush>();

        out.clear();

        out.emplace_back().as_circle();
        out.emplace_back().as_square();
        out.emplace_back().as_ellipse_horizontal();
        out.emplace_back().as_ellipse_vertical();
        out.emplace_back().as_rectangle_horizontal();
        out.emplace_back().as_rectangle_vertical();

        auto files = get_all_files_in_directory(get_resource_path() + "brushes", false, false);
        std::sort(files.begin(), files.end(), [](FileDescriptor& a, FileDescriptor& b) -> bool {
            return a.get_name() < b.get_name();
        });

        for (auto& file : files)
        {
            auto& brush = out.emplace_back();
            auto image = Image();
            if (image.create_from_file(file.get_path()))
            {
                brush.as_custom(image);
                out.emplace_back(brush);
            }
            else
                out.pop_back();
        }

        return out;
    }

    std::vector<HSVA> state::load_default_palette_colors()
    {
        std::vector<HSVA> colors;

        const size_t n_steps = 8;
        for (size_t i = 0; i < n_steps; ++i)
        {
            float h = i / float(n_steps);

            for (float s : {0.33f, 0.66f, 1.f})
                for (float v : {0.33f, 0.66f, 1.f})
                    colors.push_back(HSVA(h, s, v, 1));
        }

        for (size_t i = 0; i < n_steps; ++i)
            colors.push_back(HSVA(0, 0, i / float(n_steps), 1));

        return colors;
    }

    ProjectState::ProjectState(Vector2i layer_resolution)
        : _layer_resolution(layer_resolution)
    {
        auto colors = state::load_default_palette_colors();
        _palette = Palette(colors);
        _primary_color = colors.at(0);
        _secondary_color = invert(colors.at(0));
        _preview_color_current = _primary_color;
        _preview_color_previous = _primary_color;

        _brushes = state::load_default_brushes();

        for (size_t x = 0; x < _layer_resolution.x; ++x)
            for (size_t y = 0; y < _layer_resolution.y; ++y)
                _selection.insert({x, y});

        _layers.emplace_back("Layer 01", layer_resolution, 1);
    }

    const Brush* ProjectState::get_current_brush() const
    {
        return &_brushes.at(_current_brush_i);
    }

    void ProjectState::set_current_brush(size_t i)
    {
        _current_brush_i = i;
        state::brush_options->signal_brush_selection_updated();
        state::canvas->signal_brush_selection_updated();
    }

    size_t ProjectState::get_n_brushes() const
    {
        return _brushes.size();
    }

    const Layer* ProjectState::get_current_layer() const
    {
        return &_layers.at(_current_layer_i);
    }

    size_t ProjectState::get_n_layers() const
    {
        return _layers.size();
    }

    const Layer::Frame* ProjectState::get_current_frame() const
    {
        return get_current_layer()->get_frame(_current_frame_i);
    }

    size_t ProjectState::get_n_frames() const
    {
        return _n_frames;
    }

    void ProjectState::set_current_layer_and_frame(size_t layer_i, size_t frame_i)
    {
        _current_layer_i = layer_i;
        _current_frame_i = frame_i;

        state::canvas->signal_layer_frame_selection_updated();
        state::layer_view->signal_layer_frame_selection_updated();
        state::frame_view->signal_layer_frame_selection_updated();
        state::animation_preview->signal_layer_frame_selection_updated();
    }

    HSVA ProjectState::get_primary_color() const
    {
        return _primary_color;
    }

    void ProjectState::set_primary_color(HSVA color)
    {
        _primary_color = color;

        state::verbose_color_picker->signal_color_selection_updated();
        state::color_picker->signal_color_selection_updated();
        state::color_swapper->signal_color_selection_updated();
        state::color_preview->signal_color_selection_updated();
        state::palette_view->signal_color_selection_updated();
        state::canvas->signal_color_selection_updated();
    }

    HSVA ProjectState::get_secondary_color() const
    {
        return _secondary_color;
    }

    void ProjectState::set_secondary_color(HSVA color)
    {
        _secondary_color = color;

        state::verbose_color_picker->signal_color_selection_updated();
        state::color_picker->signal_color_selection_updated();
        state::color_swapper->signal_color_selection_updated();
        state::color_preview->signal_color_selection_updated();
        state::palette_view->signal_color_selection_updated();
        state::canvas->signal_color_selection_updated();
    }

    const Palette& ProjectState::get_palette() const
    {
        return _palette;
    }

    void ProjectState::set_palette(const std::vector<HSVA>& colors)
    {
        _palette = Palette(colors);
        state::palette_view->signal_palette_updated();
    }

    PaletteSortMode ProjectState::get_palette_sort_mode() const
    {
        return _palette_sort_mode;
    }

    void ProjectState::set_palette_sort_mode(PaletteSortMode mode)
    {
        _palette_sort_mode = mode;
        state::palette_view->signal_palette_sort_mode_updated();
    }

    const Vector2Set<int>& ProjectState::get_selection() const
    {
        return _selection;
    }

    void ProjectState::set_selection(Vector2Set<int> selection)
    {
        _selection = selection;
        state::canvas->signal_selection_updated();
    }

    void ProjectState::select_all()
    {
        _selection.clear();
        for (size_t x = 0; x < _layer_resolution.x; ++x)
            for (size_t y = 0; y < _layer_resolution.y; ++y)
                _selection.insert({x, y});

        state::canvas->signal_selection_updated();
    }

    bool ProjectState::get_playback_active() const
    {
        return _playback_active;
    }

    void ProjectState::set_playback_active(bool b)
    {
        _playback_active = b;

        state::canvas->signal_playback_active_updated();
        state::frame_view->signal_playback_active_updated();
        state::layer_view->signal_playback_active_updated();
        state::animation_preview->signal_playback_active_updated();
    }

    bool ProjectState::get_onionskin_visible() const
    {
        return _onionskin_visible;
    }

    void ProjectState::set_onionskin_visible(bool b)
    {
        _onionskin_visible = b;
        state::canvas->signal_onionskin_visibility_updated();
        state::frame_view->signal_onionskin_visibility_updated();
    }

    size_t ProjectState::get_n_onionskin_layers() const
    {
        return _onionskin_n_layers;
    }

    void ProjectState::set_n_onionskin_layers(size_t n)
    {
        _onionskin_n_layers = n;

        state::canvas->signal_onionskin_layer_count_updated();
        state::frame_view->signal_onionskin_layer_count_updated();
    }

    void ProjectState::draw_to_layer(size_t layer_i, size_t frame_i, std::map<Vector2i, HSVA> data)
    {
        auto* frame = _layers.at(layer_i).get_frame(frame_i);
        auto* image = frame->get_image();

        for (auto& pair : data)
            image->set_pixel(pair.first.x, pair.first.y, pair.second);

        frame->update_texture();

        state::canvas->signal_image_updated();
        state::layer_view->signal_image_updated();
        state::frame_view->signal_image_updated();
        state::animation_preview->signal_image_updated();
    }
}