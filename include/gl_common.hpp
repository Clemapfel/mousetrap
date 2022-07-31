// 
// Copyright 2022 Clemens Cords
// Created on 7/15/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <array>
#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/glm.hpp>

#include <include/vector.hpp>

static inline bool GL_INITIALIZED = false;
using GLNativeHandle = GLuint;

namespace rat
{
    Vector2f get_viewport_size();

    Vector2f to_gl_position(Vector2f);
    Vector2f from_gl_position(Vector2f);

    Vector3f to_gl_position(Vector3f);
    Vector3f from_gl_position(Vector3f);

    Vector2f to_gl_distance(Vector2f);
    Vector2f from_gl_distance(Vector2f);
}

#include <src/gl_common.inl>

// #############################################################################

