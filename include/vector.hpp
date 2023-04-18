//
// Copyright 2022 Clemens Cords
// Created on 7/31/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <glm/glm.hpp>
#include <set>

namespace mousetrap
{
    /// @brief vector of 2 numbers
    /// @tparam T number type
    template<typename T>
    using Vector2 = glm::vec<2, T>;

    /// @brief vector of 2 floats
    using Vector2f = glm::vec<2, float>;

    /// @brief vector of 2 size_t
    using Vector2ui = glm::vec<2, size_t>;

    /// @brief vector of 2 int64_t
    using Vector2i = glm::vec<2, int64_t>;

    /// @brief vector of 3 numbers
    /// @tparam T number type
    template<typename T>
    using Vector3 = glm::vec<3, T>;

    /// @brief vector of 3 float
    using Vector3f = glm::vec<3, float>;

    /// @brief vector of 3 size_t
    using Vector3ui = glm::vec<3, size_t>;

    /// @brief vector of 3 int64_t
    using Vector3i = glm::vec<3, int64_t>;

    /// @brief vector of 4 numbers
    /// @tparam T number type
    template<typename T>
    using Vector4 = glm::vec<4, T>;

    /// @brief vector of 4 float
    using Vector4f = glm::vec<4, float>;

    /// @brief vector of 4 size_t
    using Vector4ui = glm::vec<4, size_t>;

    /// @brief vector of 4 int
    using Vector4i = glm::vec<4, int64_t>;

    /// @brief compare Vector2f
    /// @tparam T number type
    /// @param a
    /// @param b
    /// @return true if a.x == b.x and a.y == b.y
    template<typename T>
    bool operator==(const glm::vec<2, T>& a, const glm::vec<2, T>& b)
    {
        return a.x == b.x and a.y == b.y;
    }

    /// @brief compare Vector2f
    /// @tparam T number type
    /// @param a
    /// @param b
    /// @return true if a.x != b.x or a.y != b.y
    template<typename T>
    bool operator!=(const glm::vec<2, T>& a, const glm::vec<2, T>& b)
    {
        return not (a == b);
    }

    /// @brief compare Vector3f
    /// @tparam T number type
    /// @param a
    /// @param b
    /// @return true if a.x == b.x and a.y == b.y and a.z == b.z
    template<typename T>
    bool operator==(const glm::vec<3, T>& a, const glm::vec<3, T>& b)
    {
        return a.x == b.x and a.y == b.y and a.z == b.z;
    }

    /// @brief compare Vector3f
    /// @tparam T number type
    /// @param a
    /// @param b
    /// @return true if a.x != b.x or a.y != b.y or a.z != b.z
    template<typename T>
    bool operator!=(const glm::vec<3, T>& a, const glm::vec<3, T>& b)
    {
        return not (a == b);
    }

    /// @brief compare Vector4f
    /// @tparam T number type
    /// @param a
    /// @param b
    /// @return true if a.x == b.x and a.y == b.y and a.z == b.z and a.w == b.w
    template<typename T>
    bool operator==(const glm::vec<4, T>& a, const glm::vec<4, T>& b)
    {
        return a.x == b.x and a.y == b.y and a.z == b.z and a.w == b.w;
    }

    /// @brief compare Vector4f
    /// @tparam T number type
    /// @param a
    /// @param b
    /// @return true if a.x != b.x or a.y != b.y or a.z != b.z or a.w != b.w
    template<typename T>
    bool operator!=(const glm::vec<4, T>& a, const glm::vec<4, T>& b)
    {
        return not (a == b);
    }

    /// @brief compare two Vector2f, orders them by x, then y
    /// @tparam T number type
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

    /// @brief compare two Vector3f, orders them by x, then y, then z
    /// @tparam T number type
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

    /// @brief compare two Vector4f, orders them by x, then y, then z, then w
    /// @tparam T number type
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

    /// @brief set of Vector2
    /// @tparam number type
    template<typename T>
    using Vector2Set = std::set<Vector2<T>, Vector2Compare<T>>;

    /// @brief set of Vector2f
    using Vector2fSet = Vector2Set<float>;

    /// @brief set of Vector2i
    using Vector2iSet = Vector2Set<int64_t>;

    /// @brief set of Vector2ui
    using Vector2uiSet = Vector2Set<size_t>;

    /// @brief set of Vector2
    /// @tparam number type
    template<typename T>
    using Vector3Set = std::set<Vector3<T>, Vector3Compare<T>>;

    /// @brief set of Vector3f
    using Vector3fSet = Vector3Set<float>;

    /// @brief set of Vector3i
    using Vector3iSet = Vector3Set<int64_t>;

    /// @brief set of Vector3ui
    using Vector3uiSet = Vector3Set<size_t>;

    /// @brief set of Vector2
    /// @tparam number type
    template<typename T>
    using Vector4Set = std::set<Vector4<T>, Vector4Compare<T>>;

    /// @brief set of Vector4f
    using Vector4fSet = Vector4Set<float>;

    /// @brief set of Vector4i
    using Vector4iSet = Vector4Set<int64_t>;

    /// @brief set of Vector4ui
    using Vector4uiSet = Vector4Set<size_t>;
}