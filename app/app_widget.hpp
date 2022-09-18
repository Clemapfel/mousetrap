// 
// Copyright 2022 Clemens Cords
// Created on 9/18/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>

namespace mousetrap
{
    template<typename GtkWidget_t>
    struct AppWidget : public WidgetImplementation<GtkWidget_t>
    {
        virtual void update() = 0;
    };
}