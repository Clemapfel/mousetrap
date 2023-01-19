#include <app/algorithms.hpp>
#include <app/config_files.hpp>

namespace mousetrap
{
    /// \brief generate 1-pixel rasterized line between two texels
    std::vector<Vector2i> generate_line_points(Vector2i a, Vector2i b)
    {
        std::vector<Vector2i> out = {a, b};

        if (a.x == b.x)
        {
            auto y_min = std::min(a.y, b.y);
            auto y_max = std::max(a.y, b.y);
            out.reserve(y_max - y_min);

            for (int y = y_min; y < y_max; ++y)
                out.push_back({a.x, y});

            return out;
        }
        else if (a.y == b.x)
        {
            auto x_min = std::min(a.x, b.x);
            auto x_max = std::max(a.x, b.x);
            out.reserve(x_max - x_min);

            for (int x = x_min; x < x_max; ++x)
                out.push_back({x, a.y});

            return out;
        }

        // source:
        //  [1] https://en.wikipedia.org/wiki/Digital_differential_analyzer_(graphics_algorithm)
        //  [2] https://www.cs.virginia.edu/luther/blog/posts/492.html

        float x1 = a.x;
        float x2 = b.x;
        float y1 = a.y;
        float y2 = b.y;

        float dx = x2 - x1;
        float dy = y2 - y1;
        float slope = dy / dx;

        const int eps = 10e7; // project into int range to avoid float precision resulting in non-deterministic results

        if (abs(dx) > abs(dy))
        {
            float x_step = x1 < x2 ? 1 : -1;
            float y_step = slope * x_step;

            float y = y1;
            float x = x1;

            auto n_steps = std::max(abs(dx), abs(dy));
            out.reserve(out.size() + n_steps);

            for (size_t step = 0; step < n_steps; ++step)
            {
                if (int(glm::fract(y) * eps) >= eps / 2)
                    out.emplace_back(x, int(y+1));
                else
                    out.emplace_back(x, int(y));

                x += x_step;
                y += y_step;
            }
        }
        else if (abs(dx) < abs(dy))
        {
            float y_step = y1 < y2 ? 1 : -1;
            float x_step = (1 / slope) * y_step;

            float x = x1;
            float y = y1;

            auto n_steps = std::max(abs(dx), abs(dy));
            out.reserve(out.size() + n_steps);

            for (size_t step = 0; step < n_steps; ++step)
            {
                if (int(glm::fract(x) * eps) >= eps / 2)
                    out.emplace_back(int(x+1), y);
                else
                    out.emplace_back(int(x), y);

                x += x_step;
                y += y_step;
            }
        }
        else
        {
            int x = a.x;
            int y = a.y;

            int x_step = a.x < b.x ? 1 : -1;
            int y_step = a.y < b.y ? 1 : -1;

            size_t n_steps = abs(a.x - b.x); // same as abs(b.y - a.y)
            out.reserve(out.size() + n_steps);

            for (size_t i = 0; i < n_steps; ++i)
            {
                out.emplace_back(x, y);
                x += x_step;
                y += y_step;
            }
        }


        return out;
    }

    /// \brief generate 1-pixel rasterized circle outline
    std::vector<Vector2i> generate_circle_points(size_t width, size_t height)
    {
        std::vector<Vector2i> out;

        // source: [1] https://stackoverflow.com/questions/15474122/is-there-a-midpoint-ellipse-algorithm

        auto center = Vector2i(
                width / 2,
                height / 2
        );

        int a = width / 2.f;
        int b = height / 2.f;

        int a2 = a * a;
        int b2 = b * b;

        int x = 0;
        int y = b;
        int px = 0;
        int py = 2 * a2 * y;

        float p = (b2 - (a2 * b) + (0.25 * a2));

        auto add_points = [&](auto x, auto y)
        {
            auto x_offset = int(width % 2 == 0);
            auto y_offset = int(height % 2 == 0);

            out.emplace_back(center.x + x - x_offset, center.y + y - y_offset);
            out.emplace_back(center.x - x, center.y + y - y_offset);
            out.emplace_back(center.x + x - x_offset, center.y - y);
            out.emplace_back(center.x - x, center.y - y);
        };

        add_points(x, y);

        while (px < py)
        {
            x++;
            px += 2 * b2;

            if (p < 0)
                p += b2 + px;
            else
            {
                y--;
                py -= 2 * a2;
                p += b2 + px - py;
            }

            add_points(x, y);
        }

        p = (b2 * (x + 0.5) * (x + 0.5) + a2 * (y-1) * (y-1) - a2 * b2);

        while (y > 0)
        {
            y--;
            py -= 2 * a2;
            if (p > 0)
                p += a2 - py;
            else
            {
                x++;
                px += 2 * b2;
                p += a2 - py + px;
            }

            add_points(x, y);
        }

        return out;
    }

    /// \brief generate lines that will outline the non-0 area of an image, ordered by clockwise orientation
    OutlineVertices generate_outline_vertices(const Image& image)
    {
        OutlineVertices out;

        auto w = image.get_size().x;
        auto h = image.get_size().y;

        static auto alpha_eps = state::settings_file->get_value_as<float>("global", "alpha_epsilon");

        for (size_t x = 0; x < w; ++x)
        {
            for (size_t y = 0; y < h - 1; ++y)
            {
                auto top = image.get_pixel(x, y).a > alpha_eps;
                auto bottom = image.get_pixel(x, y + 1).a > alpha_eps;

                std::pair<Vector2i, Vector2i> to_push = {
                        {x,     y + 1},
                        {x + 1, y + 1}
                };

                if (top and not bottom)
                    out.top.push_back(to_push);
                else if (not top and bottom)
                    out.bottom.push_back(to_push);
            }
        }

        for (size_t y = 0; y < h; ++y)
        {
            for (size_t x = 0; x < w - 1; ++x)
            {
                auto left = image.get_pixel(x, y).a > alpha_eps;
                auto right = image.get_pixel(x + 1, y).a > alpha_eps;

                std::pair<Vector2i, Vector2i> to_push = {
                        {x + 1, y},
                        {x + 1, y + 1}
                };

                if (left and not right)
                    out.left.push_back(to_push);
                else if (not left and right)
                    out.right.push_back(to_push);
            }
        }

        for (size_t y = 0; y < h; ++y)
            if (image.get_pixel(0, y).a > alpha_eps)
                out.right.push_back({{0, y}, {0, y+1}});

        for (size_t y = 0; y < h; ++y)
            if (image.get_pixel(w-1, y).a > alpha_eps)
                out.left.push_back({{w, y}, {w, y+1}});

        for (size_t x = 0; x < w; ++x)
            if (image.get_pixel(x, 0).a > alpha_eps)
                out.bottom.push_back({{x, 0}, {x + 1, 0}});

        for (size_t x = 0; x < w; ++x)
            if (image.get_pixel(x, h-1).a > alpha_eps)
                out.top.push_back({{x, h}, {x + 1, h}});

        return out;
    }

    /// \brief generate lines that will outline the area described by the set of coordinates, ordered by clockwise orientation
    OutlineVertices generate_outline_vertices(const Vector2Set<int>& set)
    {
        OutlineVertices out;

        int min_x = std::numeric_limits<int>::max();
        int min_y = std::numeric_limits<int>::max();
        int max_x = std::numeric_limits<int>::min();
        int max_y = std::numeric_limits<int>::min();

        for (auto& v : set)
        {
            min_x = std::min(v.x, min_x);
            min_y = std::min(v.y, min_y);
            max_x = std::max(v.x, max_x);
            max_y = std::max(v.y, max_y);
        }

        auto w = max_x - min_x;
        auto h = max_y - min_y;

        static auto alpha_eps = state::settings_file->get_value_as<float>("global", "alpha_epsilon");

        auto is_in_set = [&](int x, int y) {
            return set.find(Vector2i(x, y)) != set.end();
        };

        for (size_t x = min_x; x <= min_x + w; ++x)
        {
            for (size_t y = min_y; y <= min_y + h - 1; ++y)
            {
                auto top = is_in_set(x, y);
                auto bottom = is_in_set(x, y + 1);

                std::pair<Vector2i, Vector2i> to_push = {
                        {x,     y + 1},
                        {x + 1, y + 1}
                };

                if (top and not bottom)
                    out.top.push_back(to_push);
                else if (not top and bottom)
                    out.bottom.push_back(to_push);
            }
        }

        for (size_t y = min_y; y <= min_y + h; ++y)
        {
            for (size_t x = min_x; x <= min_x + w - 1; ++x)
            {
                auto left = is_in_set(x, y);
                auto right = is_in_set(x + 1, y);

                std::pair<Vector2i, Vector2i> to_push = {
                        {x + 1, y},
                        {x + 1, y + 1}
                };

                if (left and not right)
                    out.left.push_back(to_push);
                else if (not left and right)
                    out.right.push_back(to_push);
            }
        }

        for (size_t y = min_y; y <= min_y + h; ++y)
            if (is_in_set(0, y))
                out.right.push_back({{0, y}, {0, y+1}});

        for (size_t y = min_y; y <= min_y + h; ++y)
            if (is_in_set(w-1, y))
                out.left.push_back({{w, y}, {w, y+1}});

        for (size_t x = min_x; x <= min_x + w; ++x)
            if (is_in_set(x, 0))
                out.bottom.push_back({{x, 0}, {x + 1, 0}});

        for (size_t x = min_x; x <= min_x + w; ++x)
            if (is_in_set(x, h-1))
                out.top.push_back({{x, h}, {x + 1, h}});

        return out;
    }

    Image generate_rectangle_outline(size_t width, size_t height, HSVA color)
    {
        auto out = Image();
        out.create(width, height, RGBA(0, 0, 0, 0));

        for (size_t x = 0; x < width; ++x)
        {
            out.set_pixel(x, 0, color);
            out.set_pixel(x, height - 1, color);
        }

        for (size_t y = 1; y < height - 1; ++y)
        {
            out.set_pixel(0, y, color);
            out.set_pixel(width - 1, y, color);
        }

        return out;
    }

    Image generate_rectangle_filled(size_t width, size_t height, HSVA color)
    {
        auto out = Image();
        out.create(width, height, color);
        return out;
    }

    Image generate_circle_outline(size_t width, size_t height, HSVA color)
    {
        const auto points = generate_circle_points(width, height);
        auto out = Image();
        out.create(width, height, RGBA(0, 0, 0, 0));

        for (auto& point : points)
            out.set_pixel(point.x, point.y, color);

        return out;
    }

    Image generate_circle_filled(size_t width, size_t height, HSVA color)
    {
        const auto points = generate_circle_points(width, height);

        // horizontal stripe decomposition: y -> x_left, x_right
        std::map<int, std::pair<int, int>> ranges;

        for (auto& point : points)
        {
            auto it = ranges.find(point.y);
            if (it != ranges.end())
            {
                (*it).second.first = std::min<int>((*it).second.first, point.x);
                (*it).second.second = std::max<int>((*it).second.second, point.x);
            }
            else
                ranges.insert({point.y, {point.x, point.x}});
        }

        auto out = Image();
        out.create(width, height, RGBA(0, 0, 0, 0));

        for (auto& pair : ranges)
            for (size_t x = pair.second.first; x < pair.second.second; ++x)
                out.set_pixel(x, pair.first, color);

        return out;
    }

}