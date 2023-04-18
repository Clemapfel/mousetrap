//
// Copyright 2022 Clemens Cords
// Created on 7/31/22 by clem (mail@clemens-cords.com)
//

#include <include/gl_common.hpp>
#include <iostream>

namespace mousetrap
{
    void initialize_opengl()
    {
        if (GL_INITIALIZED != true)
        {
            glewInit();
            GL_INITIALIZED = true;
        }
    }

    Vector2f to_gl_position(Vector2f in)
    {
        auto out = in;
        out.y = 1 - out.y;
        out -= 0.5;
        out *= 2;
        return out;
    }

    Vector3f to_gl_position(Vector3f in)
    {
        auto xy = to_gl_position({in.x, in.y});
        return {xy.x, xy.y, in.z};
    }

    Vector2f from_gl_position(Vector2f in)
    {
        auto out = in;
        out /= 2;
        out += 0.5;
        out.y = 1 - out.y;
        return out;
    }

    Vector3f from_gl_position(Vector3f in)
    {
        auto xy = from_gl_position({in.x, in.y});
        return {xy.x, xy.y, in.z};
    }
}
