#include <app/layer.hpp>
#include <app/algorithms.hpp>

namespace mousetrap
{
    Layer::Frame::Frame(Vector2i size)
    {
        _image.create(size.x, size.y, RGBA(0, 0, 0, 0));
        _texture.create_from_image(_image);
    }

    Image* Layer::Frame::get_image()
    {
        return &_image;
    }

    Texture* Layer::Frame::get_texture()
    {
        return &_texture;
    }

    bool Layer::Frame::get_is_keyframe() const
    {
        return _is_keyframe;
    }

    void Layer::Frame::set_is_keyframe(bool b)
    {
        _is_keyframe = b;
    }

    void Layer::Frame::update_texture()
    {
        _texture.create_from_image(_image);
    }

    Layer::Layer(const std::string& name, Vector2i size, size_t n_frames)
        : _name(name)
    {
        for (size_t i = 0; i < n_frames; ++i)
            _frames.emplace_back(size);
    }

    Layer::Frame* Layer::get_frame(size_t index)
    {
        return &_frames.at(index);
    }

    size_t Layer::get_n_frames() const
    {
        return _frames.size();
    }

    std::string Layer::get_name() const
    {
        return _name;
    }

    void Layer::set_name(const std::string& name)
    {
        _name = name;
    }

    bool Layer::get_is_locked() const
    {
        return _is_locked;
    }

    bool Layer::get_is_visible() const
    {
        return _is_visible;
    }

    void Layer::set_is_locked(bool b)
    {
        _is_locked = b;
    }

    void Layer::set_is_visible(bool b)
    {
        _is_visible = b;
    }

    float Layer::get_opacity() const
    {
        return _opacity;
    }

    void Layer::set_opacity(float v)
    {
        _opacity = glm::clamp<float>(v, 0, 1);
    }

    BlendMode Layer::get_blendmode() const
    {
        return _blend_mode;
    }

    void Layer::set_blendmode(BlendMode mode)
    {
        _blend_mode = mode;
    }

    /*
    void Layer::Frame::draw_pixel(Vector2i xy, RGBA color, BlendMode blend_mode)
    {
        if (not (xy.x >= 0 and xy.x < image->get_size().x and xy.y >= 0 and xy.y < image->get_size().y))
            return;

        auto dest = image->get_pixel(xy.x, xy.y);
        auto source = color;
        RGBA final;

        bool should_clamp = false;

        switch (blend_mode)
        {
            case BlendMode::NONE:
                final = source;
                break;

            case BlendMode::NORMAL:
            {
                final.r = source.r + dest.r * (1 - source.a);
                final.g = source.g + dest.g * (1 - source.a);
                final.b = source.b + dest.b * (1 - source.a);
                final.a = source.a + dest.a * (1 - source.a);

                if (final.r > source.r)
                    final.r = source.r;

                if (final.g > source.g)
                    final.g = source.g;

                if (final.b > source.b)
                    final.b = source.b;

                break;
            }

            case BlendMode::SUBTRACT:
                final.r = source.r - dest.r;
                final.g = source.g - dest.g;
                final.b = source.b - dest.b;
                final.a = source.a - dest.a;

                should_clamp = true;
                break;

            case BlendMode::MULTIPLY:
                final.r = source.r * dest.r;
                final.g = source.g * dest.g;
                final.b = source.b * dest.b;
                final.a = source.a * dest.a;

                should_clamp = true;
                break;

            case BlendMode::REVERSE_SUBTRACT:
                final.r = dest.r - source.r;
                final.g = dest.g - source.g;
                final.b = dest.b - source.b;
                final.a = dest.a - source.a;

                should_clamp = true;
                break;

            case BlendMode::ADD:
                final.r = source.r + dest.r;
                final.g = source.g + dest.g;
                final.b = source.b + dest.b;
                final.a = source.a + dest.a;

                should_clamp = true;
                break;

            case BlendMode::MAX:
                final.r = std::max<float>(source.r, dest.r);
                final.g = std::max<float>(source.g, dest.g);
                final.b = std::max<float>(source.b, dest.b);
                final.a = std::max<float>(source.a, dest.a);
                break;

            case BlendMode::MIN:
                final.r = std::min<float>(source.r, dest.r);
                final.g = std::min<float>(source.g, dest.g);
                final.b = std::min<float>(source.b, dest.b);
                final.a = std::min<float>(source.a, dest.a);
                break;
        }

        if (should_clamp)
        {
            final.r = glm::clamp<float>(final.r, 0, 1);
            final.g = glm::clamp<float>(final.g, 0, 1);
            final.b = glm::clamp<float>(final.b, 0, 1);
            final.a = glm::clamp<float>(final.a, 0, 1);
        }

        image->set_pixel(xy.x, xy.y, final);
    }

    void Layer::Frame::draw_line(Vector2i start, Vector2i end, RGBA color, BlendMode blend_mode)
    {
        for (auto& pos : generate_line_points(start, end))
            draw_pixel(pos, color, blend_mode);
    }

    void Layer::Frame::draw_polygon(const std::vector<Vector2i>& points, RGBA color, BlendMode blend_mode)
    {
        for (size_t i = 0; i < points.size(); ++i)
            for (auto& pos : generate_line_points(points.at(i), points.at(i + 1)))
                draw_pixel(pos, color, blend_mode);

        for (auto& pos : generate_line_points(points.back(), points.front()))
            draw_pixel(pos, color, blend_mode);
    }

    void Layer::Frame::draw_polygon_filled(const std::vector<Vector2i>& points, RGBA color, BlendMode blend_mode)
    {
        // degenerate area into horizontal bands

        struct Bounds
        {
            int64_t min;
            int64_t max;
        };

        std::map<int, Bounds> coords;
        auto add_point = [&](const Vector2i& pos) -> void
        {
            auto it = coords.find(pos.y);
            if (it == coords.end())
                coords.insert({pos.y, Bounds{pos.x, pos.x}});
            else
            {
                auto& bounds = it->second;
                bounds.min = std::min<int>(bounds.min, pos.x);
                bounds.max = std::max<int>(bounds.max, pos.x);
            }
        };

        for (size_t i = 0; i < points.size(); ++i)
            for (auto& pos : generate_line_points(points.at(i), points.at(i + 1)))
                add_point(pos);

        for (auto& pos : generate_line_points(points.back(), points.front()))
            add_point(pos);

        for (auto& pair : coords)
            for (size_t x = pair.second.min; x < pair.second.max; ++x)
                draw_pixel({x, pair.first}, color, blend_mode);
    }

    void Layer::Frame::draw_rectangle(Vector2i top_left, Vector2i bottom_right, RGBA color, size_t px, BlendMode blend_mode)
    {
        // guaranteed to only call draw_pixel the minimum number of times, no matter the input

        auto min_x = std::min(top_left.x, bottom_right.x);
        auto min_y = std::min(top_left.y, bottom_right.y);
        auto max_x = std::max(top_left.x, bottom_right.x);
        auto max_y = std::max(top_left.y, bottom_right.y);

        if (min_x < 0)
            min_x = 0;

        if (min_y < 0)
            min_y = 0;

        if (max_x >= image->get_size().x - 1)
            max_x = image->get_size().x - 1;

        if (max_y >= image->get_size().y - 1)
            max_y = image->get_size().y - 1;

        auto x = min_x;
        auto y = min_y;
        auto w = max_x - min_x;
        auto h = max_y - min_y;
        auto x_m = std::min<size_t>(px, w / 2);
        auto y_m = std::min<size_t>(px, h / 2);

        for (auto i = x; i < x + w; ++i)
            for (auto j = y; j < y + y_m; ++j)
                draw_pixel({i, j}, color, blend_mode);

        for (auto i = x; i < x + w; ++i)
            for (auto j = y + h - y_m; j < y + h; ++j)
                draw_pixel({i, j}, color, blend_mode);

        for (auto i = x; y_m != h / 2 and i < x + x_m; ++i)
            for (auto j = y + y_m; j < y + h - y_m; ++j)
                draw_pixel({i, j}, color, blend_mode);

        for (auto i = x + w - x_m; y_m != h / 2 and i < x + w; ++i)
            for (auto j = y + y_m; j < y + h - y_m; ++j)
                draw_pixel({i, j}, color, blend_mode);
    }

    void Layer::Frame::draw_rectangle_filled(Vector2i top_left, Vector2i bottom_right, RGBA color, BlendMode blend_mode)
    {
        auto min_x = std::min(top_left.x, bottom_right.x);
        auto min_y = std::min(top_left.y, bottom_right.y);
        auto max_x = std::max(top_left.x, bottom_right.x);
        auto max_y = std::max(top_left.y, bottom_right.y);

        if (min_x < 0)
            min_x = 0;

        if (min_y < 0)
            min_y = 0;

        if (max_x >= image->get_size().x - 1)
            max_x = image->get_size().x - 1;

        if (max_y >= image->get_size().y - 1)
            max_y = image->get_size().y - 1;

        for (size_t x = min_x; x < max_x; ++x)
            for (size_t y = min_y; y < max_y; ++y)
                draw_pixel({x, y}, color, blend_mode);
    }
     */
}
