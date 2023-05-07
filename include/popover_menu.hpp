// 
// Copyright 2022 Clemens Cords
// Created on 10/19/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>
#include <include/menu_model.hpp>

namespace mousetrap
{
    class PopoverMenu : public WidgetImplementation<GtkPopoverMenu>
    {
        public:
            PopoverMenu(MenuModel*);

        protected:
            void refresh_widgets();

        private:
            MenuModel* _model;
    };
}

