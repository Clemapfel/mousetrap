// 
// Copyright 2022 Clemens Cords
// Created on 7/31/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>

namespace rat
{
    class Container : public Widget
    {
        public:
            template<typename T>
            void add(T);
    };
}

// ###

namespace rat
{
    template<typename T>
    void Container::add(T in)
    {
        gtk_container_add(GTK_CONTAINER(get_native()), (GtkWidget*) in);
    }
}