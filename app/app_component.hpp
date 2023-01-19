// 
// Copyright 2022 Clemens Cords
// Created on 9/18/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>

namespace mousetrap
{
    struct AppComponent
    {
        virtual void update() = 0;
        virtual operator Widget*() = 0;

        operator GtkWidget*();
    };
}

// ###
namespace mousetrap
{
    inline AppComponent::operator GtkWidget*()
    {
        return operator Widget*()->operator GtkWidget*();
    }
}
