//
// Created by clem on 2/25/23.
//

#include <app/canvas.hpp>

namespace mousetrap
{
    Canvas::ControlBar::ControlBar(Canvas* owner)
        : _owner(owner)
    {
        _gid_visible
    }

    Canvas::ControlBar::operator Widget*()
    {
        return &_main;
    }
}
