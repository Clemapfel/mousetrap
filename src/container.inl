// 
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

namespace mousetrap
{
    template<typename T>
    void Container::add(T in)
    {
        gtk_container_add(GTK_CONTAINER(get_native()), (GtkWidget*) in);
    }
}