// 
// Copyright 2022 Clemens Cords
// Created on 9/3/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/flow_box.hpp>
#include <include/box.hpp>
#include <include/image_display.hpp>
#include <include/toggle_button.hpp>

#include <app/global_state.hpp>

namespace mousetrap
{
    class Toolbox : public Widget
    {
        private:
            struct ToolboxIconGroup
            {
                std::vector<state::ToolboxTool> _tools;
                std::vector<ImageDisplay> _images;
                std::vector<ToggleButton> _buttons;
                Box _box;

                void expand();
                void retract();
            };

    };
}