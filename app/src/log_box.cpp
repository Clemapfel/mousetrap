//
// Created by clem on 3/8/23.
//

#include <app/log_box.hpp>
#include <app/project_state.hpp>

namespace mousetrap
{
    LogBox::LogBox()
    {
        _main.set_margin(state::margin_unit * 0.5);
    }

    LogBox::operator Widget*()
    {
        return &_main;
    }
}
