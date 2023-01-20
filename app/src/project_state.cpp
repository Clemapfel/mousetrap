#include <app/project_state.hpp>
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
#include <app/project_state.hpp>
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
    std::deque<Brush> state::load_default_brushes(const std::string& path)
    {
        auto out = std::deque<Brush>();

        out.clear();

        out.emplace_back().as_circle();
        out.emplace_back().as_square();
        out.emplace_back().as_ellipse_horizontal();
        out.emplace_back().as_ellipse_vertical();
        out.emplace_back().as_rectangle_horizontal();
        out.emplace_back().as_rectangle_vertical();

        auto files = get_all_files_in_directory(path, false, false);
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

        _brushes = state::load_default_brushes(get_resource_path() + "brushes");

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
        if (state::brush_options)
            state::brush_options->signal_brush_selection_changed();

        if (state::canvas)
            state::canvas->signal_brush_selection_changed();
    }

    size_t ProjectState::get_n_brushes() const
    {
        return _brushes.size();
    }

    const Brush* ProjectState::get_brush(size_t i) const
    {
        return &_brushes.at(i);
    }

    float ProjectState::get_brush_opacity() const
    {
        return _brush_opacity;
    }

    void ProjectState::set_brush_opacity(float v)
    {
        _brush_opacity = v;

        if (state::brush_options)
            state::brush_options->signal_brush_selection_changed();

        if (state::canvas)
            state::canvas->signal_brush_selection_changed();
    }

    void ProjectState::set_brush_size(size_t v)
    {
        _brush_size = v;

        if (state::brush_options)
            state::brush_options->signal_brush_selection_changed();

        if (state::canvas)
            state::canvas->signal_brush_selection_changed();
    }

    size_t ProjectState::get_brush_size() const
    {
        return _brush_size;
    }

    ToolID ProjectState::get_active_tool() const
    {
        return _active_tool;
    }

    void ProjectState::set_active_tool(ToolID id)
    {
        _active_tool = id;

        if (state::canvas)
            state::canvas->signal_active_tool_changed();

        if (state::toolbox)
            state::toolbox->signal_active_tool_changed();
    }

    const Layer* ProjectState::get_current_layer() const
    {
        return &_layers.at(_current_layer_i);
    }

    size_t ProjectState::get_current_frame_index() const
    {
        return _current_frame_i;
    }

    size_t ProjectState::get_current_layer_index() const
    {
        return _current_layer_i;
    }

    const Layer* ProjectState::get_layer(size_t i) const
    {
        return &_layers.at(i);
    }

    const Layer::Frame* ProjectState::get_frame(size_t layer_i, size_t frame_i) const
    {
        return _layers.at(layer_i).get_frame(frame_i);
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

        if (state::canvas)
            state::canvas->signal_layer_frame_selection_changed();

        if (state::layer_view)
            state::layer_view->signal_layer_frame_selection_changed();

        if (state::frame_view)
            state::frame_view->signal_layer_frame_selection_changed();

        if (state::animation_preview)
            state::animation_preview->signal_layer_frame_selection_changed();
    }

    void ProjectState::add_layer(int i)
    {
        if (i < -1)
            i = -1;

        // TODO

        if (state::canvas)
            state::canvas->signal_layer_frame_selection_changed();

        if (state::layer_view)
            state::layer_view->signal_layer_frame_selection_changed();

        if (state::frame_view)
            state::frame_view->signal_layer_frame_selection_changed();

        if (state::animation_preview)
            state::animation_preview->signal_layer_frame_selection_changed();
    }

    void ProjectState::delete_layer(size_t i)
    {
        // TODO

        if (state::canvas)
            state::canvas->signal_layer_frame_selection_changed();

        if (state::layer_view)
            state::layer_view->signal_layer_frame_selection_changed();

        if (state::frame_view)
            state::frame_view->signal_layer_frame_selection_changed();

        if (state::animation_preview)
            state::animation_preview->signal_layer_frame_selection_changed();
    }

    void ProjectState::set_layer_visible(size_t i, bool b)
    {
        _layers.at(i).set_is_visible(i);

        if (state::canvas)
            state::canvas->signal_layer_properties_changed();

        if (state::layer_view)
            state::layer_view->signal_layer_properties_changed();

        if (state::frame_view)
            state::frame_view->signal_layer_properties_changed();

        if (state::animation_preview)
            state::animation_preview->signal_layer_properties_changed();
    }

    void ProjectState::set_layer_opacity(size_t i, float v)
    {
        _layers.at(i).set_opacity(v);

        if (state::canvas)
            state::canvas->signal_layer_properties_changed();

        if (state::layer_view)
            state::layer_view->signal_layer_properties_changed();

        if (state::frame_view)
            state::frame_view->signal_layer_properties_changed();

        if (state::animation_preview)
            state::animation_preview->signal_layer_properties_changed();
    }

    void ProjectState::set_layer_locked(size_t i, bool b)
    {
        _layers.at(i).set_is_locked(i);

        if (state::canvas)
            state::canvas->signal_layer_properties_changed();

        if (state::layer_view)
            state::layer_view->signal_layer_properties_changed();

        if (state::frame_view)
            state::frame_view->signal_layer_properties_changed();

        if (state::animation_preview)
            state::animation_preview->signal_layer_properties_changed();
    }

    void ProjectState::set_layer_blend_mode(size_t i, BlendMode mode)
    {
        _layers.at(i).set_blend_mode(mode);

        if (state::canvas)
            state::canvas->signal_layer_properties_changed();

        if (state::layer_view)
            state::layer_view->signal_layer_properties_changed();

        if (state::frame_view)
            state::frame_view->signal_layer_properties_changed();

        if (state::animation_preview)
            state::animation_preview->signal_layer_properties_changed();
    }

    void ProjectState::set_layer_name(size_t i, const std::string& name)
    {
        _layers.at(i).set_name(name);

        if (state::canvas)
            state::canvas->signal_layer_properties_changed();

        if (state::layer_view)
            state::layer_view->signal_layer_properties_changed();

        if (state::frame_view)
            state::frame_view->signal_layer_properties_changed();

        if (state::animation_preview)
            state::animation_preview->signal_layer_properties_changed();
    }

    Vector2ui ProjectState::get_layer_resolution() const
    {
        return _layer_resolution;
    }

    HSVA ProjectState::get_primary_color() const
    {
        return _primary_color;
    }

    void ProjectState::set_primary_color(HSVA color)
    {
        _primary_color = color;

        if (state::verbose_color_picker)
            state::verbose_color_picker->signal_color_selection_changed();

        if (state::color_picker)
            state::color_picker->signal_color_selection_changed();

        if (state::color_swapper)
            state::color_swapper->signal_color_selection_changed();

        if (state::color_preview)
            state::color_preview->signal_color_selection_changed();

        if (state::palette_view)
            state::palette_view->signal_color_selection_changed();

        if (state::canvas)
            state::canvas->signal_color_selection_changed();
    }

    HSVA ProjectState::get_secondary_color() const
    {
        return _secondary_color;
    }

    void ProjectState::set_secondary_color(HSVA color)
    {
        _secondary_color = color;

        if (state::verbose_color_picker)
            state::verbose_color_picker->signal_color_selection_changed();

        if (state::color_picker)
            state::color_picker->signal_color_selection_changed();

        if (state::color_swapper)
            state::color_swapper->signal_color_selection_changed();

        if (state::color_preview)
            state::color_preview->signal_color_selection_changed();

        if (state::palette_view)
            state::palette_view->signal_color_selection_changed();

        if (state::canvas)
            state::canvas->signal_color_selection_changed();
    }

    HSVA ProjectState::get_preview_color_current() const
    {
        return _preview_color_current;
    }

    void ProjectState::set_preview_colors(HSVA current, HSVA previous)
    {
        _preview_color_current = current;
        _preview_color_previous = previous;

        if (state::color_preview)
            state::color_preview->signal_color_selection_changed();
    }

    HSVA ProjectState::get_preview_color_previous() const
    {
        return _preview_color_previous;
    }

    const Palette& ProjectState::get_palette() const
    {
        return _palette;
    }

    void ProjectState::set_palette(const std::vector<HSVA>& colors)
    {
        _palette = Palette(colors);

        if (state::palette_view)
            state::palette_view->signal_palette_updated();
    }

    PaletteSortMode ProjectState::get_palette_sort_mode() const
    {
        return _palette_sort_mode;
    }

    void ProjectState::set_palette_sort_mode(PaletteSortMode mode)
    {
        _palette_sort_mode = mode;
        state::palette_view->signal_palette_sort_mode_changed();
    }

    const Vector2Set<int>& ProjectState::get_selection() const
    {
        return _selection;
    }

    void ProjectState::set_selection(Vector2Set<int> selection)
    {
        _selection = selection;

        if (state::canvas)
            state::canvas->signal_selection_changed();
    }

    void ProjectState::select_all()
    {
        _selection.clear();
        for (size_t x = 0; x < _layer_resolution.x; ++x)
            for (size_t y = 0; y < _layer_resolution.y; ++y)
                _selection.insert({x, y});

        if (state::canvas)
            state::canvas->signal_selection_changed();
    }

    bool ProjectState::get_playback_active() const
    {
        return _playback_active;
    }

    void ProjectState::set_playback_active(bool b)
    {
        _playback_active = b;

        if (state::canvas)
            state::canvas->signal_playback_toggled();

        if (state::frame_view)
            state::frame_view->signal_playback_toggled();

        if (state::layer_view)
            state::layer_view->signal_playback_toggled();

        if (state::animation_preview)
            state::animation_preview->signal_playback_toggled();
    }

    bool ProjectState::get_onionskin_visible() const
    {
        return _onionskin_visible;
    }

    void ProjectState::set_onionskin_visible(bool b)
    {
        _onionskin_visible = b;
        if (state::canvas)
            state::canvas->signal_onionskin_visibility_toggled();

        if (state::frame_view)
        state::frame_view->signal_onionskin_visibility_toggled();
    }

    size_t ProjectState::get_n_onionskin_layers() const
    {
        return _onionskin_n_layers;
    }

    void ProjectState::set_n_onionskin_layers(size_t n)
    {
        _onionskin_n_layers = n;

        if (state::canvas)
            state::canvas->signal_onionskin_layer_count_changed();

        if (state::frame_view)
            state::frame_view->signal_onionskin_layer_count_changed();
    }

    void ProjectState::draw_to_layer(size_t layer_i, size_t frame_i, std::map<Vector2i, HSVA> data)
    {
        auto* frame = _layers.at(layer_i).get_frame(frame_i);
        auto* image = frame->get_image();

        for (auto& pair : data)
            image->set_pixel(pair.first.x, pair.first.y, pair.second);

        frame->update_texture();

        if (state::canvas)
            state::canvas->signal_layer_image_updated();

        if (state::layer_view)
            state::layer_view->signal_layer_image_updated();

        if (state::frame_view)
            state::frame_view->signal_layer_image_updated();

        if (state::animation_preview)
            state::animation_preview->signal_layer_image_updated();
    }
}