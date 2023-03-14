//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 3/12/23
//

#pragma once

#include <mousetrap.hpp>
#include <set>

namespace mousetrap
{
    namespace detail
    {
        struct DrawDataSort
        {
            bool operator()(const std::pair <Vector2i, HSVA>& a, const std::pair <Vector2i, HSVA>& b) const noexcept
            {
                return (a.first.x == b.first.x) ? a.first.y < b.first.y : a.first.x < b.first.x;
            }
        };
    }
    using DrawData = std::set<std::pair<Vector2i, HSVA>, detail::DrawDataSort>;
}
