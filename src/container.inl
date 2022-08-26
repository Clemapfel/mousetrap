// 
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

#include <vector>

namespace mousetrap
{
    template<typename Widget_t> requires (not std::is_same_v<Widget_t, GtkWidget>)
    void Container::add(Widget_t* in)
    {
        add(in->get_native());
    }

    /*
    void Container::remove(Widget* out)
    {
        remove(out->get_native());
    }
     */

    void Container::clear()
    {
        std::vector<GtkWidget*> to_remove;

        auto* current = gtk_widget_get_first_child(get_native());
        to_remove.push_back(current);

        while (current != nullptr)
        {
            current = gtk_widget_get_next_sibling(current);
            to_remove.push_back(current);
        }

        for (auto* w : to_remove)
            remove(w);
    }
}