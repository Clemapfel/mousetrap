// 
// Copyright 2022 Clemens Cords
// Created on 7/31/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <glm/glm.hpp>
#include <set>

namespace mousetrap
{
    template<typename T>
    using Vector2 = glm::vec<2, T>;

    using Vector2f = glm::vec<2, float>;
    using Vector2ui = glm::vec<2, size_t>;
    using Vector2i = glm::vec<2, int64_t>;

    template<typename T>
    using Vector3 = glm::vec<3, T>;

    using Vector3f = glm::vec<3, float>;
    using Vector3ui = glm::vec<3, size_t>;
    using Vector3i = glm::vec<3, int64_t>;

    template<typename T>
    using Vector4 = glm::vec<4, T>;

    using Vector4f = glm::vec<4, float>;
    using Vector4ui = glm::vec<4, size_t>;
    using Vector4i = glm::vec<4, int64_t>;

    template<typename T>
    bool operator==(const glm::vec<2, T>& a, const glm::vec<2, T>& b)
    {
        return a.x == b.x and a.y == b.y;
    }

    template<typename T>
    bool operator!=(const glm::vec<2, T>& a, const glm::vec<2, T>& b)
    {
        return not (a == b);
    }

    template<typename T>
    bool operator==(const glm::vec<3, T>& a, const glm::vec<3, T>& b)
    {
        return a.x == b.x and a.y == b.y and a.z == b.z;
    }

    template<typename T>
    bool operator!=(const glm::vec<3, T>& a, const glm::vec<3, T>& b)
    {
        return not (a == b);
    }

    template<typename T>
    bool operator==(const glm::vec<4, T>& a, const glm::vec<4, T>& b)
    {
        return a.x == b.x and a.y == b.y and a.z == b.z and a.w == b.w;
    }

    template<typename T>
    bool operator!=(const glm::vec<4, T>& a, const glm::vec<4, T>& b)
    {
        return not (a == b);
    }

    template<typename T>
    struct Vector2Compare
    {
        bool operator()(const Vector2<T>& a, const Vector2<T>& b) const
        {
            if (a.x == b.x)
                return a.y < b.y;
            else
                return a.x < b.x;
        }
    };

    template<typename T>
    struct Vector3Compare
    {
        bool operator()(const Vector3<T>& a, const Vector3<T>& b) const
        {
            if (a.x == b.x)
            {
                if (a.y == b.y)
                    return a.z < b.z;
                else
                    return a.y < b.y;
            }
            else
                return a.x < b.x;
        }
    };

    template<typename T>
    struct Vector4Compare
    {
        bool operator()(const Vector4<T>& a, const Vector4<T>& b) const
        {
            if (a.x == b.x)
            {
                if (a.y == b.y)
                {
                    if (a.z == b.z)
                        return a.w < b.w;
                    else
                        return a.z < b.z;
                }
                else
                    return a.y < b.y;
            }
            else
                return a.x < b.x;
        }
    };

    template<typename T>
    using Vector2Set = std::set<Vector2<T>, Vector2Compare<T>>;
    using Vector2fSet = Vector2Set<float>;
    using Vector2iSet = Vector2Set<int64_t>;
    using Vector2uiSet = Vector2Set<size_t>;

    template<typename T>
    using Vector3Set = std::set<Vector3<T>, Vector3Compare<T>>;
    using Vector3fSet = Vector3Set<float>;
    using Vector3iSet = Vector3Set<int64_t>;
    using Vector3uiSet = Vector3Set<size_t>;

    template<typename T>
    using Vector4Set = std::set<Vector4<T>, Vector4Compare<T>>;
    using Vector4fSet = Vector4Set<float>;
    using Vector4iSet = Vector4Set<int64_t>;
    using Vector4uiSet = Vector4Set<size_t>;

}