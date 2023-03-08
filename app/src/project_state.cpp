#include <app/project_state.hpp>
#include <app/brush_options.hpp>
#include <app/animation_preview.hpp>
#include <app/brush_options.hpp>
#include <app/bubble_log_area.hpp>
#include <app/canvas.hpp>
#include <app/color_preview.hpp>
#include <app/color_swapper.hpp>
#include <app/config_files.hpp>
#include <app/frame_view.hpp>
#include <app/project_state.hpp>
#include <app/image_transform_dialog.hpp>
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
#include <app/canvas_export.hpp>
#include <app/scale_canvas_dialog.hpp>
#include <app/resize_canvas_dialog.hpp>
#include <app/log_box.hpp>

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
                brush.as_custom(image);
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

        _brushes = state::load_default_brushes(get_default_brush_directory());

        _n_frames = 10;
        _layer_resolution = {50, 50};
        _layers.emplace_front(new Layer("Layer #" + std::to_string(_layers.size()), _layer_resolution, _n_frames));

        for (size_t i = 0; i < 10; ++i)
        {
            auto image = Image();
            image.create_from_file(get_resource_path() + "example_animation/0" + std::to_string(i) + ".png");

            auto* frame = _layers.at(0)->get_frame(i);

            for (size_t x = 0; x < image.get_size().x; ++x)
                for (size_t y = 0; y < image.get_size().y; ++y)
                    frame->set_pixel(x, y, image.get_pixel(x, y));

            frame->update_texture();
        }

        for (size_t x = 0; x < _layer_resolution.x; ++x)
            for (size_t y = 0; y < _layer_resolution.y; ++y)
                _selection.insert({x, y});


        set_save_path(std::tmpnam(nullptr));

        /*

        constexpr float COLOR = 2;
        constexpr float ZERO = 0;
        constexpr float HALF = 0.5;
        constexpr float ONE = 1;

        auto add_debug_layer = [&](float rgb_mode, float alpha_mode)
        {
            _layers.emplace_front(new Layer("Layer #" + std::to_string(_layers.size()), _layer_resolution, _n_frames));

            for (size_t frame_i = 0; frame_i < _n_frames; ++frame_i)
            {
                std::vector<std::pair<Vector2i, HSVA>> to_draw;

                if (rgb_mode == COLOR)
                {
                    for (size_t x = 0; x < _layer_resolution.x; ++x)
                        for (size_t y = 0; y < _layer_resolution.y; ++y)
                            to_draw.push_back({
                                Vector2i(x, y),
                                x == y ? HSVA(0, 0, 0, 1) : HSVA(rand() / float(RAND_MAX),
                                1,
                                rand() / float(RAND_MAX) + (x > 0.5 * layer_resolution.x ? 0.5 : 0.2),
                                glm::clamp<float>(alpha_mode, 0, 1))
                            });
                }
                else
                {
                    for (size_t x = 0; x < _layer_resolution.x; ++x)
                        for (size_t y = 0; y < _layer_resolution.y; ++y)
                            to_draw.push_back({Vector2i(x, y), RGBA(rgb_mode, rgb_mode, rgb_mode, glm::clamp<float>(alpha_mode, 0, 1)).operator HSVA()});
                }

                draw_to_cell({0, frame_i}, to_draw);
            }
        };

        for (size_t x = 0; x < _layer_resolution.x; ++x)
            for (size_t y = 0; y < _layer_resolution.y; ++y)
                _selection.insert({x, y});

        //add_debug_layer(ZERO, ONE);
        //add_debug_layer(HALF, ONE);
        add_debug_layer(ONE, HALF);
        add_debug_layer(COLOR, ONE);

        add_debug_layer(ZERO, ONE);
        add_debug_layer(HALF, ONE);
        add_debug_layer(ONE, ONE);
        add_debug_layer(COLOR, ONE);
        add_debug_layer(COLOR, HALF);
        */
    }

    const Brush* ProjectState::get_current_brush() const
    {
        return &_brushes.at(_current_brush_i);
    }

    void ProjectState::set_current_brush(size_t i)
    {
        _current_brush_i = i;
        _brushes.at(_current_brush_i).set_size(_brush_size);
        signal_brush_selection_changed();
    }

    size_t ProjectState::get_current_brush_index() const
    {
        return _current_brush_i;
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

    const std::deque<Brush>& ProjectState::get_brushes() const
    {
        return _brushes;
    }

    void ProjectState::remove_brush(size_t i)
    {
        auto to_erase = _brushes.at(i);
        if (to_erase.get_shape() != BrushShape::CUSTOM)
        {
            state::bubble_log->send_message("Brush `" + to_erase.get_name() + "` is a default brush and cannot be deleted", InfoMessageType::WARNING);
            return;
        }

        _brushes.erase(_brushes.begin() + i);

        signal_brush_set_updated();
    }

    void ProjectState::add_brush(Brush brush)
    {
        _brushes.push_back(brush);
        signal_brush_set_updated();
    }

    void ProjectState::load_default_brushes()
    {
        auto path = get_default_brush_directory();
        _brushes = state::load_default_brushes(path);

        signal_brush_set_updated();
        state::bubble_log->send_message("Loaded set of default brushes from `" + path + "`");
    }

    std::string ProjectState::get_default_brush_directory()
    {
        return get_resource_path() + "brushes";
    }

    void ProjectState::set_brush_opacity(float v)
    {
        _brush_opacity = v;
        signal_brush_selection_changed();
    }

    void ProjectState::set_brush_size(size_t v)
    {
        _brush_size = v;
        _brushes.at(_current_brush_i).set_size(_brush_size);
        signal_brush_selection_changed();
    }

    float ProjectState::get_bucket_fill_eps() const
    {
        return _bucket_fill_eps;
    }

    void ProjectState::set_bucket_fill_eps(float eps)
    {
        _bucket_fill_eps = eps;
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
        signal_active_tool_changed();
    }

    const Layer* ProjectState::get_current_layer() const
    {
        return _layers.at(_current_layer_i);
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
        return _layers.at(i);
    }

    const Layer::Frame* ProjectState::get_frame(size_t layer_i, size_t frame_i) const
    {
        return _layers.at(layer_i)->get_frame(frame_i);
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

    const Texture* ProjectState::get_cell_texture(size_t layer_i, size_t frame_i)
    {
        auto i = frame_i;
        while (i > 0 and not _layers.at(layer_i)->get_frame(i)->get_is_keyframe())
            i--;

        return _layers.at(layer_i)->get_frame(i)->get_texture();
    }

    void ProjectState::set_current_layer_and_frame(size_t layer_i, size_t frame_i)
    {
        if (layer_i >= _layers.size())
        {
            std::cerr << "[ERROR] In ProjectState::set_current_layer_and_frame: Layer index " << layer_i << " is out of bounds for project with " << _layers.size() << " layers" << std::endl;
            return;
        }

        if (frame_i >= _n_frames)
        {
            std::cerr << "[ERROR] In ProjectState::set_current_layer_and_frame: Frame index " << layer_i << " is out of bounds for project with " << _n_frames << " frames" << std::endl;
            return;
        }

        _current_layer_i = layer_i;
        _current_frame_i = frame_i;

        signal_layer_frame_selection_changed();
    }

    void ProjectState::add_layer(int i)
    {
        if (i < -1)
            i = -1;

        if (i > int(_layers.size() - 1))
            i = _layers.size() - 1;

        _layers.emplace(_layers.begin() + (1 + i), new Layer("New Layer #" + std::to_string(new_layer_count++ + 1), _layer_resolution, _n_frames));
        signal_layer_count_changed();
    }

    void ProjectState::duplicate_layer(int create_above, size_t duplicate_from_i)
    {
        const auto& duplicate_from = *_layers.at(duplicate_from_i);
        auto& new_layer = *_layers.emplace(_layers.begin() + (1 + create_above), new Layer(duplicate_from.get_name() + " (Copy)", _layer_resolution, _n_frames));

        for (size_t frame_i = 0; frame_i < _n_frames; ++frame_i)
        {
            auto* from = duplicate_from.get_frame(frame_i);
            auto* to = new_layer->get_frame(frame_i);

            for (size_t x = 0; x < _layer_resolution.x; ++x)
                for (size_t y = 0; y < _layer_resolution.y; ++y)
                    to->set_pixel(x, y, from->get_pixel(x, y));

            new_layer->get_frame(frame_i)->update_texture();
        }

        signal_layer_count_changed();
    }

    void ProjectState::swap_layers(size_t a_i, size_t b_i)
    {
        if (a_i > _layers.size() or b_i > _layers.size())
        {
            std::stringstream str;
            str << "Attempting to swap layers " << a_i << " and " << b_i << " but there are only " << _layers.size() << " layers";
            state::bubble_log->send_message(str.str(), InfoMessageType::ERROR);
            return;
        }

        auto* a = _layers.at(a_i);
        auto* b = _layers.at(b_i);

        auto a_copy = Layer(*_layers.at(a_i));

        a->set_name(b->get_name());
        a->set_is_locked(b->get_is_locked());
        a->set_is_visible(b->get_is_visible());
        a->set_opacity(b->get_opacity());
        a->set_blend_mode(b->get_blend_mode());

        b->set_name(a_copy.get_name());
        b->set_is_locked(a_copy.get_is_locked());
        b->set_is_visible(a_copy.get_is_visible());
        b->set_opacity(a_copy.get_opacity());
        b->set_blend_mode(a_copy.get_blend_mode());

        for (size_t i = 0; i < _n_frames; ++i)
        {
            auto* a_frame = a->get_frame(i);
            auto* b_frame = b->get_frame(i);

            for (size_t x = 0; x < _layer_resolution.x; ++x)
            {
                for (size_t y = 0; y < _layer_resolution.y; ++y)
                {
                    a_frame->set_pixel(x, y, b_frame->get_pixel(x, y));
                    b_frame->set_pixel(x, y, a_copy.get_frame(i)->get_pixel(x, y));
                }
            }

            a_frame->update_texture();
            b_frame->update_texture();
        }

        signal_layer_properties_changed();
        signal_layer_image_updated();
    }

    void ProjectState::delete_layer(size_t i)
    {
        if (_layers.size() == 1)
        {
            state::bubble_log->send_message("Attempting to delete last remaining layer. Deletion prevented", InfoMessageType::ERROR);
            return;
        }

        i = glm::clamp<size_t>(i, 0, _layers.size() - 1);

        auto* to_delete = _layers.at(i);
        _layers.erase(_layers.begin() + i);

        if (_current_layer_i > 0)
            _current_layer_i -= 1;

        signal_layer_count_changed();
        delete to_delete;
    }

    void ProjectState::new_layer_from(int above, const std::set<size_t>& from_layer_is, bool delete_froms)
    {
        if (state::canvas_export == nullptr)
        {
            std::cerr << "[ERROR] In ProjectState::new_layer_from: Attempting to merge layers but state::canvas_export is not yet initialized" << std::endl;
            return;
        }

        above = glm::clamp<int>(above, -1, _layers.size() - 1);

        std::stringstream new_name;
        new_name << "Merged Layer #" << merged_layer_count++;

        Layer* new_layer = new Layer(new_name.str() , _layer_resolution, _n_frames);

        const size_t frame_i_before = get_current_frame_index();
        const size_t layer_i_before = get_current_layer_index();
        for (size_t frame_i = 0; frame_i < _n_frames; ++frame_i)
        {
            auto image = state::canvas_export->merge_layers(from_layer_is, frame_i);
            auto* new_frame = new_layer->get_frame(frame_i);

            for (size_t x = 0; x < _layer_resolution.x; ++x)
                for (size_t y = 0; y < _layer_resolution.y; ++y)
                    new_frame->set_pixel(x, y, image.get_pixel(x, y));

            new_frame->update_texture();
        }

        _layers.emplace(_layers.begin() + (1 + above), new_layer);

        if (not delete_froms)
        {
            signal_layer_count_changed();
            return;
        }

        // delete layers, indices adjusted

        std::deque<size_t> to_delete_is;
        for (auto i : from_layer_is)
            to_delete_is.push_front(i > (1 + above) ? i + 1 : i);

        std::vector<Layer*> to_delete;
        for (auto i : to_delete_is)
        {
            to_delete.push_back(_layers.at(i));
            _layers.erase(_layers.begin() + i);
        }

        if (_current_layer_i >= _layers.size())
            _current_layer_i = _layers.size() - 1;

        signal_layer_count_changed();

        for (auto* layer : to_delete)
            delete layer;
    }

    void ProjectState::add_frame(int after, bool is_keyframe)
    {
        if (after < -1)
            after = -1;

        if (after > int(_n_frames - 1))
            after = _n_frames - 1;

        auto frame_i = after+1;
        for (auto& layer : _layers)
        {
            auto* added = layer->add_frame(_layer_resolution, frame_i);
            added->set_is_keyframe(is_keyframe);
        }

        _n_frames += 1;

        signal_layer_count_changed();
        signal_layer_frame_selection_changed();
    }

    void ProjectState::swap_frames(size_t a, size_t b)
    {
        for (size_t layer_i = 0; layer_i < _layers.size(); ++layer_i)
        {
            auto* a_frame = _layers.at(layer_i)->get_frame(a);
            auto* b_frame = _layers.at(layer_i)->get_frame(b);

            auto a_frame_img = Image();
            a_frame_img.create(_layer_resolution.x, _layer_resolution.y);
            for (size_t x = 0; x < _layer_resolution.x; ++x)
                for (size_t y = 0; y < _layer_resolution.y; ++y)
                    a_frame_img.set_pixel(x, y, a_frame->get_pixel(x, y));

            for (size_t x = 0; x < _layer_resolution.x; ++x)
            {
                for (size_t y = 0; y < _layer_resolution.y; ++y)
                {
                    a_frame->set_pixel(x, y, b_frame->get_pixel(x, y));
                    b_frame->set_pixel(x, y, a_frame_img.get_pixel(x, y));
                }
            }

            a_frame->update_texture();
            b_frame->update_texture();
        }

        signal_layer_image_updated();
    }

    void ProjectState::duplicate_frame(int after, size_t duplicate_from)
    {
        for (size_t layer_i = 0; layer_i < _layers.size(); ++layer_i)
        {
            auto* layer = _layers.at(layer_i);
            Image image = Image();
            image.create(_layer_resolution.x, _layer_resolution.y);

            for (size_t x = 0; x < _layer_resolution.x; ++x)
                for (size_t y = 0; y < _layer_resolution.y; ++y)
                    image.set_pixel(x, y, layer->get_frame(duplicate_from)->get_pixel(x, y));

            auto* inserted = layer->add_frame(_layer_resolution, after + 1);

            for (size_t x = 0; x < _layer_resolution.x; ++x)
                for (size_t y = 0; y < _layer_resolution.y; ++y)
                    inserted->set_pixel(x, y, image.get_pixel(x, y));

            inserted->update_texture();
        }

        _n_frames += 1;
        signal_layer_count_changed();
    }

    void ProjectState::delete_frame(size_t i)
    {
        for (auto* layer : _layers)
            layer->delete_frame(i);

        _n_frames -= 1;
        if (_current_frame_i >= i and i != 0)
            _current_frame_i -= 1;

        signal_layer_count_changed();
        signal_layer_frame_selection_changed();
    }

    void ProjectState::new_frame_from(int after, const std::set<size_t>& from_layer_is, bool delete_froms)
    {
        std::cerr << "In ProjectState::new_frame_from: TODO" << std::endl;
    }

    Vector2i ProjectState::get_cell_offset(CellPosition position) const
    {
        return _layers.at(position.x)->get_frame(position.y)->get_offset();
    }

    void ProjectState::set_cell_offset(CellPosition position, Vector2i offset)
    {
        auto* frame = _layers.at(position.x)->get_frame(position.y);
        frame->set_offset(offset);
        frame->update_texture();
        signal_layer_image_updated();
    }

    CellPosition ProjectState::get_current_cell_position() const
    {
        return CellPosition{_current_layer_i, _current_frame_i};
    }

    void ProjectState::overwrite_cell_image(CellPosition position, const Image& image)
    {
        auto* frame = _layers.at(position.x)->get_frame(position.y);
        frame->overwrite_image(image);
        frame->update_texture();
        signal_layer_image_updated();
    }

    void ProjectState::set_layer_visible(size_t i, bool b)
    {
        _layers.at(i)->set_is_visible(b);

        signal_layer_properties_changed();
    }

    void ProjectState::set_layer_opacity(size_t i, float v)
    {
        _layers.at(i)->set_opacity(v);

        signal_layer_properties_changed();
    }

    void ProjectState::set_layer_locked(size_t i, bool b)
    {
        _layers.at(i)->set_is_locked(b);

        signal_layer_properties_changed();
    }

    void ProjectState::set_layer_blend_mode(size_t i, BlendMode mode)
    {
        _layers.at(i)->set_blend_mode(mode);

        signal_layer_properties_changed();
    }

    void ProjectState::set_layer_name(size_t i, const std::string& name)
    {
        _layers.at(i)->set_name(name);

        signal_layer_properties_changed();
    }

    void ProjectState::set_cell_is_key(Vector2ui cell_ij, bool b)
    {
        size_t layer_i = cell_ij.x;
        size_t frame_i = cell_ij.y;
        _layers.at(layer_i)->get_frame(frame_i)->set_is_keyframe(b);
        signal_layer_properties_changed();
    }

    Vector2ui ProjectState::get_layer_resolution() const
    {
        return _layer_resolution;
    }

    void ProjectState::resize_canvas(Vector2ui new_size, Vector2i offset)
    {
        for (size_t layer_i = 0; layer_i < _layers.size(); ++layer_i)
        {
            for (size_t frame_i = 0; frame_i < _n_frames; ++frame_i)
            {
                auto* frame = _layers.at(layer_i)->get_frame(frame_i);
                auto image = Image();
                image.create(_layer_resolution.x, _layer_resolution.y);

                for (size_t x = 0; x < _layer_resolution.x; ++x)
                    for (size_t y = 0; y < _layer_resolution.y; ++y)
                        image.set_pixel(x, y, frame->get_pixel(x, y));

                image = image.as_cropped(offset.x, offset.y, new_size.x, new_size.y);
                frame->overwrite_image(image);
                frame->set_size(image.get_size());
                frame->update_texture();
            }
        }

        _layer_resolution = new_size;
        signal_layer_resolution_changed();
    }

    void ProjectState::scale_canvas(Vector2ui new_size, GdkInterpType interpolation_type)
    {
        for (size_t layer_i = 0; layer_i < _layers.size(); ++layer_i)
        {
            for (size_t frame_i = 0; frame_i < _n_frames; ++frame_i)
            {
                auto* frame = _layers.at(layer_i)->get_frame(frame_i);

                auto image = Image();
                image.create(_layer_resolution.x, _layer_resolution.y);

                for (size_t x = 0; x < _layer_resolution.x; ++x)
                    for (size_t y = 0; y < _layer_resolution.y; ++y)
                        image.set_pixel(x, y, frame->get_pixel(x, y));
                image = image.as_scaled(new_size.x, new_size.y, interpolation_type);

                frame->overwrite_image(image);
                frame->set_size(image.get_size());
                frame->update_texture();
            }
        }

        _layer_resolution = new_size;
        signal_layer_resolution_changed();
    }

    HSVA ProjectState::get_primary_color() const
    {
        return _primary_color;
    }

    void ProjectState::set_primary_color(HSVA color)
    {
        _primary_color = color;
        signal_color_selection_changed();
    }

    HSVA ProjectState::get_secondary_color() const
    {
        return _secondary_color;
    }

    void ProjectState::set_secondary_color(HSVA color)
    {
        _secondary_color = color;
        signal_color_selection_changed();
    }

    void ProjectState::set_primary_and_secondary_color(HSVA primary, HSVA secondary)
    {
        _primary_color = primary;
        _secondary_color = secondary;

        signal_color_selection_changed();
    }

    void ProjectState::set_color_offset(float h, float s, float v, float r, float g, float b, float a)
    {
        _color_offset = {h, s, v, r, g, b, a};
        signal_color_offset_changed();
    }

    const std::array<float, 7> ProjectState::get_color_offset() const
    {
        return _color_offset;
    }

    void ProjectState::set_color_offset_apply_scope(ApplyScope scope)
    {
        _color_offset_apply_scope = scope;
        signal_color_offset_changed();
    }

    ApplyScope ProjectState::get_color_offset_apply_scope() const
    {
        return _color_offset_apply_scope;
    }

    void ProjectState::apply_color_offset()
    {
        auto& offset = _color_offset;

        auto apply_to_frame = [&](Layer::Frame* frame)
        {
            for (size_t y = 0; y < _layer_resolution.x; ++y)
            {
                for (size_t x = 0; x < _layer_resolution.y; ++x)
                {
                    auto as_hsva = frame->get_pixel(x, y).operator HSVA();
                    as_hsva.h = glm::fract<float>(as_hsva.h + offset.at(0));
                    as_hsva.s = glm::clamp<float>(as_hsva.s + offset.at(1), 0, 1);
                    as_hsva.v = glm::clamp<float>(as_hsva.v + offset.at(2), 0, 1);

                    auto as_rgba = as_hsva.operator RGBA();
                    as_rgba.r = glm::clamp<float>(as_rgba.r + offset.at(3), 0, 1);
                    as_rgba.g = glm::clamp<float>(as_rgba.g + offset.at(4), 0, 1);
                    as_rgba.b = glm::clamp<float>(as_rgba.b + offset.at(5), 0, 1);
                    as_rgba.a = glm::clamp<float>(as_rgba.a + offset.at(6), 0, 1);

                    frame->set_pixel(x, y, as_rgba);
                }
            }
        };

        auto& scope = _color_offset_apply_scope;
        if (scope == CURRENT_CELL)
        {
            auto* frame = _layers.at(_current_layer_i)->get_frame(_current_frame_i);
            apply_to_frame(frame);
            frame->update_texture();
        }
        else if (scope == CURRENT_LAYER)
        {
            for (size_t frame_i = 0; frame_i < _n_frames; ++frame_i)
            {
                auto* frame = _layers.at(_current_layer_i)->get_frame(frame_i);
                apply_to_frame(frame);
                frame->update_texture();
            }
        }
        else if (scope == CURRENT_FRAME)
        {
            for (size_t layer_i = 0; layer_i < _layers.size(); ++layer_i)
            {
                auto* frame = _layers.at(layer_i)->get_frame(_current_frame_i);
                apply_to_frame(frame);
                frame->update_texture();
            }
        }
        else if (scope == ApplyScope::EVERYWHERE)
        {
            for (size_t layer_i = 0; layer_i < _layers.size(); ++layer_i)
            {
                for (size_t frame_i = 0; frame_i < _n_frames; ++frame_i)
                {
                    auto* frame = _layers.at(layer_i)->get_frame(frame_i);
                    apply_to_frame(frame);
                    frame->update_texture();
                }
            }
        }
        else if (scope == ApplyScope::SELECTION)
        {
            std::cerr << "[ERROR] In ProjectState::apply_color_transform: TODO SELECTION" << std::endl;
        }

        signal_layer_image_updated();
    }

    void ProjectState::color_to_grayscale(ApplyScope scope)
    {
        auto offset_before = _color_offset;
        auto offset_scope_before = _color_offset_apply_scope;

        _color_offset = {0, -1, 0, 0, 0, 0, 0};
        _color_offset_apply_scope = scope;

        apply_color_offset();

        _color_offset = offset_before;
        _color_offset_apply_scope = offset_scope_before;

        signal_layer_image_updated();
    }

    void ProjectState::color_invert(ApplyScope scope)
    {
        auto apply_to_frame = [&](Layer::Frame* frame)
        {
            for (size_t y = 0; y < _layer_resolution.y; ++y)
            {
                for (size_t x = 0; x < _layer_resolution.x; ++x)
                {
                    auto as_rgba = frame->get_pixel(x, y);
                    as_rgba.r = 1 - as_rgba.r;
                    as_rgba.g = 1 - as_rgba.g;
                    as_rgba.b = 1 - as_rgba.b;

                    frame->set_pixel(x, y, as_rgba);
                }
            }
        };

        if (scope == CURRENT_CELL)
        {
            auto* frame = _layers.at(_current_layer_i)->get_frame(_current_frame_i);
            apply_to_frame(frame);
            frame->update_texture();
        }
        else if (scope == CURRENT_LAYER)
        {
            for (size_t frame_i = 0; frame_i < _n_frames; ++frame_i)
            {
                auto* frame = _layers.at(_current_layer_i)->get_frame(frame_i);
                apply_to_frame(frame);
                frame->update_texture();
            }
        }
        else if (scope == CURRENT_FRAME)
        {
            for (size_t layer_i = 0; layer_i < _layers.size(); ++layer_i)
            {
                auto* frame = _layers.at(layer_i)->get_frame(_current_frame_i);
                apply_to_frame(frame);
                frame->update_texture();
            }
        }
        else if (scope == ApplyScope::EVERYWHERE)
        {
            for (size_t layer_i = 0; layer_i < _layers.size(); ++layer_i)
            {
                for (size_t frame_i = 0; frame_i < _n_frames; ++frame_i)
                {
                    auto* frame = _layers.at(layer_i)->get_frame(frame_i);
                    apply_to_frame(frame);
                    frame->update_texture();
                }
            }
        }
        else if (scope == ApplyScope::SELECTION)
        {
            std::cerr << "[ERROR] In ProjectState::color_invert: TODO SELECTION" << std::endl;
        }

        signal_layer_image_updated();
    }

    HSVA ProjectState::get_preview_color_current() const
    {
        return _preview_color_current;
    }

    void ProjectState::set_preview_colors(HSVA current, HSVA previous)
    {
        _preview_color_current = current;
        _preview_color_previous = previous;

        signal_color_selection_changed();
    }

    HSVA ProjectState::get_preview_color_previous() const
    {
        return _preview_color_previous;
    }

    const Palette& ProjectState::get_palette() const
    {
        return _palette;
    }

    const Palette& ProjectState::get_default_palette() const
    {
        if (not _default_palette.load_from(_default_palette_path))
            std::cerr << "[ERROR] In ProjectState::get_default_palette: Unable to load palette from " + _default_palette_path << std::endl;

        return _default_palette;
    }

    void ProjectState::set_default_palette(const std::vector<HSVA>& colors)
    {
        _default_palette = Palette(colors);

        if (not _default_palette.save_to(_default_palette_path))
            std::cerr << "[ERROR] In ProjectState::set_default_palette: Unable to save default palette to " + _default_palette_path << std::endl;
        else
            state::bubble_log->send_message("Saved current palette as default");
    }

    void ProjectState::set_palette(const std::vector<HSVA>& colors)
    {
        _palette = Palette(colors);

        signal_palette_updated();
    }

    PaletteSortMode ProjectState::get_palette_sort_mode() const
    {
        return _palette_sort_mode;
    }

    void ProjectState::set_palette_sort_mode(PaletteSortMode mode)
    {
        _palette_sort_mode = mode;
        signal_palette_sort_mode_changed();
    }

    void ProjectState::set_palette_editing_enabled(bool b)
    {
        _palette_editing_enabled = b;
        signal_palette_editing_toggled();
    }

    bool ProjectState::get_palette_editing_enabled() const
    {
        return _palette_editing_enabled;
    }

    const Vector2iSet& ProjectState::get_selection() const
    {
        return _selection;
    }

    void ProjectState::set_selection(Vector2iSet selection)
    {
        _selection = selection;

        signal_selection_changed();
    }

    void ProjectState::select_all()
    {
        _selection.clear();
        for (size_t x = 0; x < _layer_resolution.x; ++x)
            for (size_t y = 0; y < _layer_resolution.y; ++y)
                _selection.insert({x, y});

        signal_selection_changed();
    }

    bool ProjectState::get_playback_active() const
    {
        return _playback_active;
    }

    void ProjectState::set_playback_active(bool b)
    {
        _playback_active = b;
        signal_playback_toggled();
    }

    bool ProjectState::get_onionskin_visible() const
    {
        return _onionskin_visible;
    }

    void ProjectState::set_onionskin_visible(bool b)
    {
        _onionskin_visible = b;
        signal_onionskin_visibility_toggled();
    }

    size_t ProjectState::get_n_onionskin_layers() const
    {
        return _onionskin_n_layers;
    }

    void ProjectState::set_n_onionskin_layers(size_t n)
    {
        _onionskin_n_layers = n;
        signal_onionskin_layer_count_changed();
    }

    void ProjectState::draw_to_cell(Vector2ui cell_ij, const ProjectState::DrawData& data)
    {
        size_t layer_i = cell_ij.x;
        size_t frame_i = cell_ij.y;
        auto* frame = _layers.at(layer_i)->get_frame(frame_i);

        for (auto& pair : data)
            frame->set_pixel(pair.first.x, pair.first.y, pair.second);

        frame->update_texture();
        signal_layer_image_updated();
    }

    void ProjectState::copy_to_cell(CellPosition a, CellPosition b)
    {
        auto* from = _layers.at(a.x)->get_frame(a.y);
        auto* to = _layers.at(b.x)->get_frame(b.y);

        for (size_t x = 0; x < _layer_resolution.x; ++x)
            for (size_t y = 0; y < _layer_resolution.y; ++y)
                to->set_pixel(x, y, from->get_pixel(x, y));

        to->update_texture();

        signal_layer_image_updated();
    }

    void ProjectState::swap_cells(CellPosition a, CellPosition b)
    {
        auto* a_frame = _layers.at(a.x)->get_frame(a.y);
        auto* b_frame = _layers.at(b.x)->get_frame(b.y);

        Image a_img;
        Image b_img;

        a_img.create(_layer_resolution.x, _layer_resolution.y);
        b_img.create(_layer_resolution.x, _layer_resolution.y);

        for (size_t x = 0; x < _layer_resolution.x; ++x)
        {
            for (size_t y = 0; y < _layer_resolution.y; ++y)
            {
                a_img.set_pixel(x, y, a_frame->get_pixel(x, y));
                b_img.set_pixel(x, y, b_frame->get_pixel(x, y));
            }
        }

        for (size_t x = 0; x < _layer_resolution.x; ++x)
        {
            for (size_t y = 0; y < _layer_resolution.y; ++y)
            {
                a_frame->set_pixel(x, y, b_img.get_pixel(x, y));
                b_frame->set_pixel(x, y, a_img.get_pixel(x, y));
            }
        }

        a_frame->update_texture();
        b_frame->update_texture();

        signal_layer_image_updated();
    }

    void ProjectState::set_fps(float fps)
    {
        _playback_fps = fps;
        signal_playback_fps_changed();
    }

    float ProjectState::get_fps() const
    {
        return _playback_fps;
    }

    void ProjectState::set_image_flip(bool flip_horizontally, bool flip_vertically)
    {
        _image_flip.flip_horizontally = flip_horizontally;
        _image_flip.flip_vertically = flip_vertically;
        signal_image_flip_changed();
    }

    ProjectState::flip_state ProjectState::get_image_flip() const
    {
        return _image_flip;
    }

    void ProjectState::set_image_flip_apply_scope(ApplyScope scope)
    {
        _image_flip_apply_scope = scope;
        signal_image_flip_changed();
    }

    ApplyScope ProjectState::get_image_flip_apply_scope() const
    {
        return _image_flip_apply_scope;
    }

    void ProjectState::apply_image_flip()
    {
        auto apply_to_frame = [&](Layer::Frame* frame)
        {
            auto image = Image();
            image.create(_layer_resolution.x, _layer_resolution.y);

            for (size_t x = 0; x < _layer_resolution.x; ++x)
                for (size_t y = 0; y < _layer_resolution.y; ++y)
                    image.set_pixel(x, y, frame->get_pixel(x, y));

            image = image.as_flipped(_image_flip.flip_horizontally, _image_flip.flip_vertically);

            for (size_t x = 0; x < _layer_resolution.x; ++x)
                for (size_t y = 0; y < _layer_resolution.y; ++y)
                    frame->set_pixel(x, y, image.get_pixel(x, y));
        };

        auto& scope = _image_flip_apply_scope;
        if (scope == CURRENT_CELL)
        {
            auto* frame = _layers.at(_current_layer_i)->get_frame(_current_frame_i);
            apply_to_frame(frame);
            frame->update_texture();
        }
        else if (scope == CURRENT_LAYER)
        {
            for (size_t frame_i = 0; frame_i < _n_frames; ++frame_i)
            {
                auto* frame = _layers.at(_current_layer_i)->get_frame(frame_i);
                apply_to_frame(frame);
                frame->update_texture();
            }
        }
        else if (scope == CURRENT_FRAME)
        {
            for (size_t layer_i = 0; layer_i < _layers.size(); ++layer_i)
            {
                auto* frame = _layers.at(layer_i)->get_frame(_current_frame_i);
                apply_to_frame(frame);
                frame->update_texture();
            }
        }
        else if (scope == ApplyScope::EVERYWHERE)
        {
            for (size_t layer_i = 0; layer_i < _layers.size(); ++layer_i)
            {
                for (size_t frame_i = 0; frame_i < _n_frames; ++frame_i)
                {
                    auto* frame = _layers.at(layer_i)->get_frame(frame_i);
                    apply_to_frame(frame);
                    frame->update_texture();
                }
            }
        }
        else if (scope == ApplyScope::SELECTION)
        {
            std::cerr << "[ERROR] In ProjectState::apply_image_flip: TODO SELECTION" << std::endl;
        }

        signal_layer_image_updated();

        _image_flip.flip_horizontally = false;
        _image_flip.flip_vertically = false;
        signal_image_flip_changed();
    }

    void ProjectState::rotate_clockwise()
    {
        auto rotate  = [](Image* image)
        {
            auto out = Image();
            out.create(image->get_size().y, image->get_size().x);

            for(size_t x = 0; x < image->get_size().x; x++)
                for(size_t y = 0; y < image->get_size().y; y++)
                    out.set_pixel(y, x, image->get_pixel(image->get_size().x - 1 - x, y));

            return out;
        };

        for (size_t layer_i = 0; layer_i < _layers.size(); ++layer_i)
        {
            for (size_t frame_i = 0; frame_i < _n_frames; ++frame_i)
            {
                auto* frame = _layers.at(layer_i)->get_frame(frame_i);
                auto image = Image();
                image.create(_layer_resolution.x, _layer_resolution.y);

                for (size_t x = 0; x < _layer_resolution.x; ++x)
                    for (size_t y = 0; y < _layer_resolution.y; ++y)
                        image.set_pixel(x, y, frame->get_pixel(x, y));

                image = rotate(&image);

                for (size_t x = 0; x < _layer_resolution.x; ++x)
                    for (size_t y = 0; y < _layer_resolution.y; ++y)
                        frame->set_pixel(x, y, image.get_pixel(x, y));

                frame->update_texture();
            }
        }

        _layer_resolution = {_layer_resolution.y, _layer_resolution.x};
        signal_layer_resolution_changed();
    }

    void ProjectState::rotate_counterclockwise()
    {
        auto apply_to_frame  = [](Image* image)
        {
            auto out = Image();
            out.create(image->get_size().y, image->get_size().x);

            for(size_t x = 0; x < image->get_size().x; x++)
                for(size_t y = 0; y < image->get_size().y; y++)
                    out.set_pixel(y, x, image->get_pixel(x, image->get_size().y - 1 - y));

            *image = out;
        };

        auto rotate = [](Image* image)
        {
            auto out = Image();
            out.create(image->get_size().y, image->get_size().x);

            for(size_t x = 0; x < image->get_size().x; x++)
                for(size_t y = 0; y < image->get_size().y; y++)
                    out.set_pixel(y, x, image->get_pixel(image->get_size().x - 1 - x, y));

            return out;
        };

        for (size_t layer_i = 0; layer_i < _layers.size(); ++layer_i)
        {
            for (size_t frame_i = 0; frame_i < _n_frames; ++frame_i)
            {
                auto* frame = _layers.at(layer_i)->get_frame(frame_i);
                auto image = Image();
                image.create(_layer_resolution.x, _layer_resolution.y);

                for (size_t x = 0; x < _layer_resolution.x; ++x)
                    for (size_t y = 0; y < _layer_resolution.y; ++y)
                        image.set_pixel(x, y, frame->get_pixel(x, y));

                image = rotate(&image);

                for (size_t x = 0; x < _layer_resolution.x; ++x)
                    for (size_t y = 0; y < _layer_resolution.y; ++y)
                        frame->set_pixel(x, y, image.get_pixel(x, y));

                frame->update_texture();
            }
        }

        auto before = _layer_resolution;
        _layer_resolution.x = before.y;
        _layer_resolution.y = before.x;

        signal_layer_resolution_changed();
        signal_layer_image_updated();
    }

    void ProjectState::set_cursor_position(Vector2i position)
    {
        _cursor_position = position;
        signal_cursor_position_changed();
    }

    Vector2i ProjectState::get_cursor_position() const
    {
        return _cursor_position;
    }

    void ProjectState::set_save_path(const std::string& path)
    {
        _save_path = path;
        signal_save_path_changed();
    }

    const std::string& ProjectState::get_save_path() const
    {
        return _save_path;
    }

    void ProjectState::signal_brush_selection_changed()
    {
        if (state::brush_options)
            state::brush_options->signal_brush_selection_changed();

        if (state::canvas)
            state::canvas->signal_brush_selection_changed();
    }

    void ProjectState::signal_brush_set_updated()
    {
        if (state::brush_options)
            state::brush_options->signal_brush_set_updated();
    }

    void ProjectState::signal_color_selection_changed()
    {
        if (state::verbose_color_picker)
            state::verbose_color_picker->signal_color_selection_changed();

        if (state::color_swapper)
            state::color_swapper->signal_color_selection_changed();

        if (state::color_preview)
            state::color_preview->signal_color_selection_changed();

        if (state::palette_view)
            state::palette_view->signal_color_selection_changed();

        if (state::canvas)
            state::canvas->signal_color_selection_changed();
    }

    void ProjectState::signal_palette_updated()
    {
        if (state::palette_view)
            state::palette_view->signal_palette_updated();
    }

    void ProjectState::signal_palette_sort_mode_changed()
    {
        state::palette_view->signal_palette_sort_mode_changed();
    }

    void ProjectState::signal_palette_editing_toggled()
    {
        if (state::palette_view)
            state::palette_view->signal_palette_editing_toggled();
    }

    void ProjectState::signal_selection_changed()
    {
        if (state::canvas)
            state::canvas->signal_selection_changed();
    }

    void ProjectState::signal_onionskin_visibility_toggled()
    {
        if (state::canvas)
            state::canvas->signal_onionskin_visibility_toggled();

        if (state::frame_view)
            state::frame_view->signal_onionskin_visibility_toggled();
    }

    void ProjectState::signal_onionskin_layer_count_changed()
    {
        if (state::canvas)
            state::canvas->signal_onionskin_layer_count_changed();

        if (state::frame_view)
            state::frame_view->signal_onionskin_layer_count_changed();
    }

    void ProjectState::signal_layer_frame_selection_changed()
    {
        if (state::canvas)
            state::canvas->signal_layer_frame_selection_changed();

        if (state::layer_view)
            state::layer_view->signal_layer_frame_selection_changed();

        if (state::frame_view)
            state::frame_view->signal_layer_frame_selection_changed();

        if (state::animation_preview)
            state::animation_preview->signal_layer_frame_selection_changed();

        if (state::log_box)
            state::log_box->signal_layer_frame_selection_changed();
    }

    void ProjectState::signal_layer_image_updated()
    {
        if (state::canvas)
            state::canvas->signal_layer_image_updated();

        if (state::layer_view)
            state::layer_view->signal_layer_image_updated();

        if (state::frame_view)
            state::frame_view->signal_layer_image_updated();

        if (state::animation_preview)
            state::animation_preview->signal_layer_image_updated();

        if (state::canvas_export)
            state::canvas_export->signal_layer_image_updated();

        if (state::log_box)
            state::log_box->signal_layer_image_updated();
    }

    void ProjectState::signal_layer_count_changed()
    {
        if (state::canvas)
            state::canvas->signal_layer_count_changed();

        if (state::layer_view)
            state::layer_view->signal_layer_count_changed();

        if (state::frame_view)
            state::frame_view->signal_layer_count_changed();

        if (state::animation_preview)
            state::animation_preview->signal_layer_count_changed();

        if (state::canvas_export)
            state::canvas_export->signal_layer_count_changed();
    }

    void ProjectState::signal_layer_resolution_changed()
    {
        if (state::canvas)
            state::canvas->signal_layer_resolution_changed();

        if (state::layer_view)
            state::layer_view->signal_layer_resolution_changed();

        if (state::frame_view)
            state::frame_view->signal_layer_resolution_changed();

        if (state::animation_preview)
            state::animation_preview->signal_layer_resolution_changed();

        if (state::canvas_export)
            state::canvas_export->signal_layer_resolution_changed();

        if (state::scale_canvas_dialog)
            state::scale_canvas_dialog->signal_layer_resolution_changed();

        if (state::resize_canvas_dialog)
            state::resize_canvas_dialog->signal_layer_resolution_changed();
    }

    void ProjectState::signal_layer_properties_changed()
    {
        if (state::canvas)
            state::canvas->signal_layer_properties_changed();

        if (state::layer_view)
            state::layer_view->signal_layer_properties_changed();

        if (state::frame_view)
            state::frame_view->signal_layer_properties_changed();

        if (state::animation_preview)
            state::animation_preview->signal_layer_properties_changed();

        if (state::canvas_export)
            state::canvas_export->signal_layer_properties_changed();
    }

    void ProjectState::signal_active_tool_changed()
    {
        if (state::canvas)
            state::canvas->signal_active_tool_changed();

        if (state::toolbox)
            state::toolbox->signal_active_tool_changed();
    }

    void ProjectState::signal_playback_toggled()
    {
        if (state::frame_view)
            state::frame_view->signal_playback_toggled();

        if (state::animation_preview)
            state::animation_preview->signal_playback_toggled();
    }

    void ProjectState::signal_playback_fps_changed()
    {
        if (state::frame_view)
            state::frame_view->signal_playback_fps_changed();

        if (state::animation_preview)
            state::animation_preview->signal_playback_fps_changed();
    }

    void ProjectState::signal_color_offset_changed()
    {
        if (state::canvas)
            state::canvas->signal_color_offset_changed();

        if (state::animation_preview)
            state::animation_preview->signal_color_offset_changed();
    }

    void ProjectState::signal_image_flip_changed()
    {
        if (state::canvas)
            state::canvas->signal_image_flip_changed();

        if (state::animation_preview)
            state::animation_preview->signal_image_flip_changed();
    }

    void ProjectState::signal_save_path_changed()
    {
        if (state::log_box)
            state::log_box->signal_save_path_changed();
    }

    void ProjectState::signal_cursor_position_changed()
    {
        if (state::canvas)
            state::canvas->signal_cursor_position_changed();

        if (state::log_box)
            state::log_box->signal_cursor_position_changed();
    }
}