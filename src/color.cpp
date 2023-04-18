//
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

#include <include/gl_common.hpp>
#include <include/color.hpp>
#include <include/log.hpp>

#include <sstream>
#include <iostream>
#include <cctype>
#include <vector>

namespace mousetrap
{
    RGBA::operator std::string() const
    {
        std::stringstream out;
        out << "RGBA(" << r << ", " << g << ", " << b << ", " << a << ")";

        return out.str();
    }

    RGBA::RGBA(float r, float g, float b, float a)
    : r(r), g(g), b(b), a(a)
    {}

    RGBA::RGBA(glm::vec4 vec)
    : r(vec[0]), g(vec[1]), b(vec[2]), a(vec[3])
    {}

    RGBA::operator glm::vec4() const
    {
        return glm::vec4(r, g, b, a);
    }

    RGBA::RGBA(HSVA hsva)
    {
        auto out = detail::hsva_to_rgba(hsva.operator glm::vec4());
        r = out[0];
        g = out[1];
        b = out[2];
        a = out[4];
    }

    bool RGBA::operator==(const RGBA& other)
    {
        return int(r * 256) == int(other.r == 256) and
        int(g * 256) == int(other.g == 256) and
        int(b * 256) == int(other.b == 256) and
        int(a * 256) == int(other.a == 256);
    }

    bool RGBA::operator!=(const RGBA& other)
    {
        return not (*this == other);
    }

    RGBA::operator HSVA() const
    {
        return HSVA(detail::rgba_to_hsva(this->operator glm::vec4()));
    }

    HSVA::operator std::string() const
    {
        auto str = std::stringstream();
        str << "HSVA(" << h << ", " << s << ", " << v << ", " << a << ")";
        return str.str();
    }

    HSVA::HSVA(float h, float s, float v, float a)
    : h(h), s(s), v(v), a(a)
    {}

    HSVA::HSVA(glm::vec4 vec)
    : h(vec[0]), s(vec[1]), v(vec[2]), a(vec[3])
    {}

    HSVA::operator glm::vec4() const
    {
        return glm::vec4(h, s, v, a);
    }

    HSVA::HSVA(RGBA rgba)
    {
        auto out = detail::rgba_to_hsva(rgba.operator glm::vec4());
        h = out[0];
        s = out[1];
        v = out[2];
        a = out[3];
    }

    HSVA::operator RGBA() const
    {
        return RGBA(detail::hsva_to_rgba(this->operator glm::vec4()));
    }

    bool HSVA::operator==(const HSVA& other)
    {
        return int(h * 256) == int(other.h == 256) and
        int(s * 256) == int(other.s == 256) and
        int(v * 256) == int(other.v == 256) and
        int(a * 256) == int(other.a == 256);
    }

    bool HSVA::operator!=(const HSVA& other)
    {
        return not (*this == other);
    }

    glm::vec4 detail::rgba_to_hsva(glm::vec4 in)
    {
        const float r = in.r;
        const float g = in.g;
        const float b = in.b;
        const float a = in.a;

        float h, s, v;

        auto min = r < g ? r : g;
        min = min  < b ? min  : b;

        auto max = r > g ? r : g;
        max = max  > b ? max  : b;

        auto delta = max - min;

        if (delta == 0)
            h = 0;
        else if (max == r)
            h = 60 * (fmod(((g - b) / delta), 6));
        else if (max == g)
            h = 60 * (((b - r) / delta) + 2);
        else if (max == b)
            h = 60 * (((r - g) / delta) + 4);

        if (max == 0)
            s = 1;
        else
            s = delta / max;

        v = max;

        if (h < 0)
            h += 360.f;

        return glm::vec4(h / 360.f, s, v, a);
    }

    glm::vec4 detail::hsva_to_rgba(glm::vec4 in)
    {
        const float h = in[0] * 360;
        const float s = in[1];
        const float v = in[2];
        const float a = in[3];

        float c = v * s;
        float h_2 = h / 60.f;
        float x = c * (1 - std::fabs(std::fmod(h_2, 2) - 1));

        Vector3f rgb;

        if (0 <= h_2 and h_2 < 1)
        {
            rgb = Vector3f(c, x, 0);
        }
        else if (1 <= h_2 and h_2 < 2)
        {
            rgb = Vector3f(x, c, 0);
        }
        else if (2 <= h_2 and h_2 < 3)
        {
            rgb = Vector3f(0, c, x);
        }
        else if (3 <= h_2 and h_2 < 4)
        {
            rgb = Vector3f(0, x, c);
        }
        else if (4 <= h_2 and h_2 < 5)
        {
            rgb = Vector3f(x, 0, c);
        }
        else if (5 <= h_2 and h_2 <= 6)
        {
            rgb = Vector3f(c, 0, x);
        }

        auto m = v - c;

        rgb.r += m;
        rgb.g += m;
        rgb.b += m;

        return glm::vec4(rgb.r, rgb.g, rgb.b, a);
    }

    /*
    glm::vec4 rgba_to_cmyk(glm::vec4 in)
    {
        float r = in[0];
        float g = in[1];
        float b = in[2];

        float c = 1 - r;
        float m = 1 - g;
        float y = 1 - b;

        float k = std::min<float>(std::min<float>(c, m), y);
        return glm::vec4(c - k, m - k, y - k, k);
    }

    glm::vec4 cmyk_to_rgba(glm::vec4 in)
    {
        float c = in[0];
        float y = in[1];
        float m = in[2];
        [[maybe_unused]] float k = in[3];

        return glm::vec4(
        1 - c,
        1 - y,
        1 - m,
        1
        );

    }

    glm::vec4 hsva_to_cmyk(glm::vec4 in)
    {
        return rgba_to_cmyk(detail::hsva_to_rgba(in));
    }

    glm::vec4 cmyk_to_hsva(glm::vec4 in)
    {
        return detail::rgba_to_hsva(cmyk_to_rgba(in));
    }
     */

    RGBA html_code_to_rgba(const std::string& text)
    {
        static auto hex_char_to_int = [](char c) -> uint8_t
        {
            if (c == '0')
                return 0;

            if (c == '1')
                return 1;

            if (c == '2')
                return 2;

            if (c == '3')
                return 3;

            if (c == '4')
                return 4;

            if (c == '5')
                return 5;

            if (c == '6')
                return 6;

            if (c == '7')
                return 7;

            if (c == '8')
                return 8;

            if (c == '9')
                return 9;

            if (c == 'A')
                return 10;

            if (c == 'B')
                return 11;

            if (c == 'C')
                return 12;

            if (c == 'D')
                return 13;

            if (c == 'E')
                return 14;

            if (c == 'F')
                return 15;

            return -1; // on error
        };

        static auto hex_component_to_int = [](int left, int right) -> uint8_t
        {
            return left * 16 + right;
        };

        std::vector<int> as_hex;
        as_hex.reserve(6);
        for (size_t i = 1; i < text.size(); ++i)
        {
            as_hex.push_back(hex_char_to_int(text.at(i)));
            if (as_hex.back() == -1)
                goto on_error;
        }

        return RGBA(
        hex_component_to_int(as_hex.at(0), as_hex.at(1)) / 255.f,
        hex_component_to_int(as_hex.at(2), as_hex.at(3)) / 255.f,
        hex_component_to_int(as_hex.at(4), as_hex.at(5)) / 255.f,
        1
        );

        on_error:

            log::warning("In html_code_to_rgba: Unable to parse code \"" + text + "\"", MOUSETRAP_DOMAIN);
            return RGBA(0, 0, 0, 1);
    }

    std::string rgba_to_html_code(RGBA in, bool show_alpha = true)
    {
        auto sanitize_code = [](std::string& in) -> bool
        {
            std::string text = "#";

            size_t start = 0;
            if (in.size() == 6)
                start = 0;
            else if (in.size() == 7)
                start = 1;
            else
                return false;

            for (size_t i = start; i < 6 + start; ++i)
            {
                auto c = in.at(i);
                if (c == 'a')
                    c = 'A';
                else if (c == 'b')
                    c = 'B';
                else if (c == 'c')
                    c = 'C';
                else if (c == 'd')
                    c = 'D';
                else if (c == 'e')
                    c = 'E';
                else if (c == 'f')
                    c = 'F';

                if (not ((c == '0') or (c == '1') or (c == '2') or (c == '3') or (c == '4') or (c == '5') or (c == '6') or (c == '7') or (c == '8') or (c == '9') or (c == 'A') or (c == 'B') or (c == 'C') or (c == 'D') or (c == 'E') or (c == 'F')))
                    return false;

                text.push_back(c);
            }

            in = text;
            return true;
        };

        in.r = glm::clamp<float>(in.r, 0.f, 1.f);
        in.g = glm::clamp<float>(in.g, 0.f, 1.f);
        in.b = glm::clamp<float>(in.b, 0.f, 1.f);

        std::stringstream r;
        r << std::hex << int(std::round(in.r * 255));

        auto r_string = r.str();
        if (r_string.size() == 1)
            r_string = "0" + r_string;

        std::stringstream g;
        g << std::hex << int(std::round(in.g * 255));

        auto g_string = g.str();
        if (g_string.size() == 1)
            g_string = "0" + g_string;

        std::stringstream b;
        b << std::hex << int(std::round(in.b * 255));

        auto b_string = b.str();
        if (b_string.size() == 1)
            b_string = "0" + b_string;

        auto out = "#" + r_string + g_string + b_string;
        sanitize_code(out);
        return out;
    }

    void quantize(RGBA&, size_t n_values_per_component)
    {
        std::cerr << "[ERROR] In Color::quantize: TODO" << std::endl;
    }

    RGBA quantize(RGBA in, size_t n_values_per_component)
    {
        RGBA out;
        out.r = (int(in.r * n_values_per_component) / float(n_values_per_component));
        out.g = (int(in.g * n_values_per_component) / float(n_values_per_component));
        out.b = (int(in.b * n_values_per_component) / float(n_values_per_component));
        out.a = (int(in.a * n_values_per_component) / float(n_values_per_component));

        return out;
    }

    HSVA quantize(HSVA in, size_t n_values_per_component)
    {
        HSVA out;
        out.h = (int(in.h * n_values_per_component) / float(n_values_per_component));
        out.s = (int(in.s * n_values_per_component) / float(n_values_per_component));
        out.v = (int(in.v * n_values_per_component) / float(n_values_per_component));
        out.a = (int(in.a * n_values_per_component) / float(n_values_per_component));

        return out;
    }

    RGBA invert(RGBA in)
    {
        return RGBA(1 - in.r, 1 - in.g, 1 - in.b, in.a);
    }

    HSVA invert(HSVA in)
    {
        return HSVA(1 - in.h, in.s, 1 - in.v, in.a);
    }

    HSVA rgba_to_hsva(RGBA in)
    {
        return in.operator HSVA();
    }

    RGBA hsva_to_rgba(HSVA in)
    {
        return in.operator RGBA();
    }
}
