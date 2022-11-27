// 
// Copyright 2022 Clemens Cords
// Created on 6/27/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <glm/glm.hpp>
#include <string>

namespace mousetrap
{
    struct RGBA;
    struct HSVA;

    /// \brief rgba
    struct RGBA
    {
        float r = 0;
        float g = 0;
        float b = 0;
        float a = 0;

        RGBA() = default;
        RGBA(float r, float g, float b, float a);

        explicit RGBA(glm::vec4);
        explicit operator glm::vec4() const;

        /// \brief convert hsv to rgb
        explicit RGBA(HSVA);

        /// \brief convert rbg to hsv
        explicit operator HSVA() const;

        float to_grayscale() const;
        explicit operator std::string();

        bool operator==(const RGBA& other);
        bool operator!=(const RGBA& other);
    };

    struct HSVA
    {
        float h = 0;
        float s = 0;
        float v = 0;
        float a = 1;

        HSVA() = default;
        HSVA(float h, float s, float v, float a);

        explicit HSVA(glm::vec4);
        explicit operator glm::vec4() const;

        /// \brief convert rgb to hsv
        explicit HSVA(RGBA);

        /// \brief convert hsv to rgb
        operator RGBA() const;

        float to_grayscale() const;
        explicit operator std::string();

        bool operator==(const HSVA& other);
        bool operator!=(const HSVA& other);
    };

    glm::vec4 rgba_to_hsva(glm::vec4);
    glm::vec4 hsva_to_rgba(glm::vec4);

    RGBA html_code_to_rgba(const std::string& code);

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

    static inline const RGBA YELLOW = RGBA(1, 232 / 255.f, 15 / 255.f, 1);
}

#include <src/colors.inl>