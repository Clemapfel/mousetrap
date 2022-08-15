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

    static inline const RGBA YELLOW = RGBA(1, 232 / 255.f, 15 / 255.f, 1);
}

#include <src/colors.inl>