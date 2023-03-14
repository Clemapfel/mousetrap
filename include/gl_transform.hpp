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
    /// \brief transform, operates in gl coordinate system
    class GLTransform
    {
        public:
            GLTransform();

            Vector2f apply_to(Vector2f gl_coords);
            Vector3f apply_to(Vector3f gl_coords);

            GLTransform combine_with(GLTransform);

            void rotate(Angle, Vector2f gl_coords);
            void set_rotation(Angle);
            void translate(Vector2f gl_offset);
            void scale(float x, float y);
            void reset();

            glm::mat4x4 transform;
    };
}

