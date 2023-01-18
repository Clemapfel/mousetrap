// 
// Copyright 2022 Clemens Cords
// Created on 7/31/22 by clem (mail@clemens-cords.com)
//

namespace mousetrap
{
    inline Vector2f get_viewport_size()
    {
        std::array<GLint, 4> viewport = {0, 0, 0, 0};
        glGetIntegerv(GL_VIEWPORT, viewport.data());
        size_t width = viewport.at(2);
        size_t height = viewport.at(3);
        return Vector2f(width, height);
    }

    inline Vector2f to_gl_position(Vector2f in)
    {
        auto out = in;
        out.y = 1 - out.y;
        out -= 0.5;
        out *= 2;
        return out;
    }

    inline Vector3f to_gl_position(Vector3f in)
    {
        auto xy = to_gl_position({in.x, in.y});
        return {xy.x, xy.y, in.z};
    }

    inline Vector2f from_gl_position(Vector2f in)
    {
        auto out = in;
        out /= 2;
        out += 0.5;
        out.y = 1 - out.y;
        return out;
    }

    inline Vector3f from_gl_position(Vector3f in)
    {
        auto xy = from_gl_position({in.x, in.y});
        return {xy.x, xy.y, in.z};
    }

    inline Vector2f to_gl_distance(Vector2f distance)
    {
        auto size = get_viewport_size();
        return Vector2f(distance.x / (size.x * 0.5) , -1 * distance.y / (size.y * 0.5));
    }

    inline Vector2f from_gl_distance(Vector2f distance)
    {
        auto size = get_viewport_size();
        return Vector2f(distance.x * (size.x * 0.5), -1 * distance.y * (size.y * 0.5));
    }
}