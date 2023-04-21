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

    namespace detail
    {
        glm::vec4 rgba_to_hsva(glm::vec4);
        glm::vec4 hsva_to_rgba(glm::vec4);
    }

    /// \brief color representation in RGBA
    struct RGBA
    {
        /// @brief red, in [0, 1]
        float r = 0;

        /// @brief green, in [0, 1]
        float g = 0;

        /// @brief blue, in [0, 1]
        float b = 0;

        /// @brief transparency, in [0, 1]
        float a = 0;

        /// @brief default ctor, initialized as RGBA(0, 0, 0, 0)
        RGBA() = default;

        /// @brief construct
        /// @param r red value, in [0, 1]
        /// @param g green value, in [0, 1]
        /// @param b blue value, in [0, 1]
        /// @param a alpha value, in [0, 1]
        RGBA(float r, float g, float b, float a);

        /// @brief construct from mousetrap::Vector4f
        explicit RGBA(glm::vec4);

        /// @brief return as mousetrap::Vector4f
        explicit operator glm::vec4() const;

        /// @brief convert hsva to rgba
        /// @param hsva
        explicit RGBA(HSVA);

        /// @brief convert rbga to hsva
        explicit operator HSVA() const;

        /// @brief serialize
        explicit operator std::string() const;

        /// @brief comparison operator
        /// @param other
        /// @returns true if all components exactly equal eachother, false otherwise. This is subject to issues stemming from float precision, consider quantizing the colors before comparison
        bool operator==(const RGBA& other);

        /// @brief comparison operator
        /// @param other
        /// @returns true if all components do not exaclty equal eachother, false otherwise. This is subject to issues stemming from float precision, consider quantizing the colors before comparison
        bool operator!=(const RGBA& other);
    };

    /// @brief color representation in HSVA
    struct HSVA
    {
        /// @brief hue component
        float h = 0;

        /// @brief saturation component
        float s = 0;

        /// @brief value component
        float v = 0;

        /// @brief transparency component
        float a = 1;

        /// @brief default ctor
        HSVA() = default;

        /// @brief construct
        /// @param h hue, in [0, 1]
        /// @param s saturation, in [0, 1]
        /// @param v value, in [0, 1]
        /// @param a alpha, in [0, 1]
        HSVA(float h, float s, float v, float a);

        explicit HSVA(glm::vec4);
        explicit operator glm::vec4() const;

        /// @brief convert rgba to hsva
        /// @param rgba
        explicit HSVA(RGBA);

        /// @brief convert hsva to rgba
        operator RGBA() const;

        /// @brief serialize
        explicit operator std::string() const;

        /// @brief comparison operator
        /// @param other
        /// @returns true if all components exactly equal eachother, false otherwise. This is subject to issues stemming from float precision, consider quantizing the colors before comparison
        bool operator==(const HSVA& other);

        /// @brief comparison operator
        /// @param other
        /// @returns true if all components do not exaclty equal eachother, false otherwise. This is subject to issues stemming from float precision, consider quantizing the colors before comparison
        bool operator!=(const HSVA& other);
    };

    /// @brief convert RGBA to HSVA
    /// @param RGBA
    /// @return HSVA
    HSVA rgba_to_hsva(RGBA);

    /// @brief convert HSAV to RGBA
    /// @param HSVA
    /// @return RGBA
    RGBA hsva_to_rgba(HSVA);

    /// @brief construct a color form an HTML code of the form #RRGGBB(AA) where AA is optimal, all components in hexadecimal
    /// @param code as string
    /// @return color
    RGBA html_code_to_rgba(const std::string& code);

    /// @brief construct HTML code of the form #RRGGBB from a color, all components in hexadecimal
    /// @param color in rbga
    /// @return code
    std::string rgba_to_html_code(RGBA);

    /// @brief round all color components to the nearest fraction
    /// @param in color in rgba
    /// @param n_values_per_component denominator of the fraction
    RGBA quantize(RGBA in, size_t n_values_per_component);

    /// @brief round all color components to the nearest fraction
    /// @param in color in hsva
    /// @param n_values_per_component denominator of the fraction
    HSVA quantize(HSVA in, size_t n_values_per_component);

    /// @brief invert a color, non-mutating
    /// @param in color in rgba
    /// @return color
    [[nodiscard]] RGBA invert(RGBA in);

    /// @brief invert a color, non-mutating
    /// @param in color in hsva
    /// @return color
    [[nodiscard]] HSVA invert(HSVA in);
}
