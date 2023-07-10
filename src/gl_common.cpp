//
// Copyright 2022 Clemens Cords
// Created on 7/31/22 by clem (mail@clemens-cords.com)
//

#include <mousetrap/gl_common.hpp>

#if MOUSETRAP_ENABLE_OPENGL_COMPONENT

#include <mousetrap/log.hpp>
#include <iostream>

namespace mousetrap::detail
{
    void mark_gl_initialized()
    {
        if (not mousetrap::GL_INITIALIZED)
            mousetrap::GL_INITIALIZED = true;
    }

    void throw_if_gl_is_uninitialized()
    {
        if (mousetrap::GL_INITIALIZED)
            return;

        log::fatal(notify_if_gl_uninitialized::message, MOUSETRAP_DOMAIN);
    }
}

namespace mousetrap
{
    Vector2f to_gl_position(Vector2f in)
    {
        return in;

        /*
        auto out = in;
        out.y = 1 - out.y;
        out -= 0.5;
        out *= 2;
        return out;
         */
    }

    Vector3f to_gl_position(Vector3f in)
    {
        auto xy = to_gl_position({in.x, in.y});
        return {xy.x, xy.y, in.z};
    }

    Vector2f from_gl_position(Vector2f in)
    {
        return in;

        /*
        auto out = in;
        out /= 2;
        out += 0.5;
        out.y = 1 - out.y;
        return out;
         */
    }

    Vector3f from_gl_position(Vector3f in)
    {
        auto xy = from_gl_position({in.x, in.y});
        return {xy.x, xy.y, in.z};
    }
}

#endif // MOUSETRAP_ENABLE_OPENGL_COMPONENT
