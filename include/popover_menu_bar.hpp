// 
// Copyright 2022 Clemens Cords
// Created on 10/19/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>
#include <include/menu_model.hpp>

namespace mousetrap
{
    class PopoverMenuBar : public WidgetImplementation<GtkPopoverMenuBar>
    {
        public:
            PopoverMenuBar(MenuModel*);
    };
}

#include <src/popover_menu_bar.inl>
