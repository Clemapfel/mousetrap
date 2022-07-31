//
// Copyright 2022 Clemens Cords
// Created on 7/15/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/angle.hpp>
#include <include/gl_common.hpp>

#include <glm/gtx/transform.hpp>

namespace rat
{
    class Transform
    {
        public:
            Transform();

            Vector2f apply_to(Vector2f gl_coords);
            Vector3f apply_to(Vector3f gl_coords);

            Transform combine_with(Transform);

            void rotate(Angle, Vector2f gl_coords);
            void set_rotation(Angle);
            void translate(Vector2f gl_offset);
            void scale(float x, float y);

            glm::mat4x4 transform;
    };
}

// #############################################################################

namespace rat
{
    Transform::Transform()
            : transform(1)
    {}

    Vector2f Transform::apply_to(Vector2f point)
    {
        return apply_to(Vector3f(point.x, point.y, 1));
    }

    Vector3f Transform::apply_to(Vector3f point)
    {
        Vector4f temp = Vector4f(point.x, point.y, point.z, 1);
        temp = transform * temp;
        return temp;
    }

    Transform Transform::combine_with(Transform other)
    {
        auto out = Transform();
        out.transform = this->transform * other.transform;
        return out;
    }

    void Transform::translate(Vector2f vec)
    {
        transform = glm::translate(transform, Vector3f(vec.x, vec.y, 0));
    }

    void Transform::rotate(Angle angle, Vector2f origin)
    {
        transform = glm::translate(transform, Vector3f(-origin.x, -origin.y, 0));
        transform = glm::rotate(transform, angle.as_radians(), glm::vec3(0, 0, 1));
        transform = glm::translate(transform, Vector3f(origin.x, origin.y, 0));
    }

    void Transform::scale(float x, float y)
    {
        transform = glm::scale(transform, Vector3f(x, y, 1));
    }
}