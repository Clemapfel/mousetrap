// 
// Copyright 2022 Clemens Cords
// Created on 8/11/22 by clem (mail@clemens-cords.com)
//

#include <include/scrollbar.hpp>

namespace mousetrap
{
    inline Scrollbar::Scrollbar(Adjustment adjustment, GtkOrientation orientation)
        : WidgetImplementation<GtkScrollbar>((GTK_SCROLLBAR(gtk_scrollbar_new(orientation, adjustment.operator GtkAdjustment*()))))
    {}

    inline void Scrollbar::reformat()
    {
        auto* adjustment = gtk_scrollbar_get_adjustment(get_native());
        auto orientation = gtk_orientable_get_orientation(GTK_ORIENTABLE(get_native()));
        g_object_ref(adjustment);

        auto* parent = gtk_widget_get_parent(GTK_WIDGET(get_native()));
        if (parent != nullptr)
            gtk_widget_unparent(GTK_WIDGET(get_native()));

        override_native(gtk_scrollbar_new(orientation, adjustment));
        std::cout << get_native() << std::endl;

        g_object_unref(adjustment);


        if (parent != nullptr)
            gtk_widget_set_parent(GTK_WIDGET(get_native()), parent);
    }

    inline void Scrollbar::set_adjustment(Adjustment& adjustment)
    {
        gtk_scrollbar_set_adjustment(get_native(), adjustment.operator GtkAdjustment*());
    }

    inline Adjustment Scrollbar::get_adjustment()
    {
        return Adjustment(gtk_scrollbar_get_adjustment(get_native()));
    }
}