//
// Copyright 2022 Clemens Cords
// Created on 7/15/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/angle.hpp>
#include <include/gl_common.hpp>

#include <glm/gtx/transform.hpp>

namespace mousetrap
{
    /// @brief transform, operates in OpenGL coordinate system
    class GLTransform
    {
        public:
            /// @brief construct a identity
            GLTransform();

            /// @brief apply transform to mousetrap::Vector2f
            /// @param gl_coords vector in 2d space
            /// @return result in 2d space
            [[nodiscard]] Vector2f apply_to(Vector2f gl_coords);

            /// @brief apply transform to mousetrap::Vector3f
            /// @param gl_coords vector in 3d space
            /// @return result in 3d space
            [[nodiscard]] Vector3f apply_to(Vector3f gl_coords);

            /// @brief combin two transforms
            /// @param other
            /// @return result of self * other
            GLTransform combine_with(GLTransform);

            /// @brief rotate transform
            /// @param angle
            /// @param gl_coords origin point of rotation
            void rotate(Angle, Vector2f gl_coords);

            /// @brief translate transform
            /// @param offset offset in 2d space
            void translate(Vector2f gl_offset);

            /// @brief scale transform
            /// @param x horizontal scale
            /// @param y vertical scale
            void scale(float x, float y);

            /// @brief make transform identity
            void reset();

            /// @brief transform data
            glm::mat4x4 transform;
    };
}

