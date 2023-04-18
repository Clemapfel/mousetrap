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

/// @brief is global GL state initialized
inline bool GL_INITIALIZED = false;

/// @brief native open GL id
using GLNativeHandle = GLuint;

namespace mousetrap
{
    /// @brief initliaze the OpenGL state, should be called after gtk_init
    void initialize_opengl();

    /// @brief convert relative widget space pos to OpenGL coordinates
    /// @param pos position in 2d space
    /// @returns mousetrap::Vector2f
    Vector2f to_gl_position(Vector2f);

    /// @brief convert OpenGL coordinates to relative widget space pose
    /// @param pos position in 2d space
    /// @returns mousetrap::Vector2f
    Vector2f from_gl_position(Vector2f);

    /// @brief convert relative widget space pos to OpenGL coordinates
    /// @param pos position in 3d space
    /// @returns mousetrap::Vector3f
    Vector3f to_gl_position(Vector3f);

    /// @brief convert OpenGL coordinates to relative widget space pose
    /// @param pos position in 2d space
    /// @returns mousetrap::Vector2f
    Vector3f from_gl_position(Vector3f);
}
