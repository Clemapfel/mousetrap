//
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

#include <vector>

#include <include/box.hpp>
#include <include/log.hpp>

namespace mousetrap
{
    Box::Box(Orientation orientation)
        : WidgetImplementation<GtkBox>(GTK_BOX(gtk_box_new((GtkOrientation) orientation, 0)))
    {}

    void Box::push_back(const Widget& widget)
    {
        auto* ptr = &widget;
        WARN_IF_SELF_INSERTION(Box::push_back, this, ptr);
        gtk_box_append(get_native(), widget.operator GtkWidget*());
    }

    void Box::push_front(const Widget& widget)
    {
        auto* ptr = &widget;
        WARN_IF_SELF_INSERTION(Box::push_back, this, ptr);
        gtk_box_prepend(get_native(), widget.operator GtkWidget*());
    }

    void Box::insert_after(const Widget& to_append, const Widget& after)
    {
        auto* ptr = &to_append;
        WARN_IF_SELF_INSERTION(Box::push_back, this, ptr);
        gtk_box_insert_child_after(
            get_native(),
            to_append.operator GtkWidget*(),
            after.operator GtkWidget*()
        );
    }

    void Box::remove(const Widget& widget)
    {
        gtk_box_remove(get_native(), widget.operator GtkWidget *());
    }

    void Box::clear()
    {
        std::vector<GtkWidget*> to_remove;
        GtkWidget* current = gtk_widget_get_first_child(GTK_WIDGET(get_native()));
        while (current != nullptr)
        {
            to_remove.push_back(current);
            current = gtk_widget_get_next_sibling(current);
        }

        for (auto* w : to_remove)
            gtk_box_remove(get_native(), w);
    }

    void Box::set_homogeneous(bool b)
    {
        gtk_box_set_homogeneous(get_native(), b);
    }

    size_t Box::get_n_items()
    {
        size_t i = 0;
        auto* child = gtk_widget_get_first_child(GTK_WIDGET(get_native()));
        while(child != nullptr)
        {
            child = gtk_widget_get_next_sibling(child);
            i++;
        }

        return i;
    }

    bool Box::get_homogeneous() const
    {
        return gtk_box_get_homogeneous(get_native());
    }

    void Box::set_spacing(float spacing)
    {
        if (spacing < 0)
            log::critical("In Box::set_spacing: Spacing cannot be negative");

        gtk_box_set_spacing(get_native(), spacing);
    }

    float Box::get_spacing() const
    {
        return gtk_box_get_spacing(get_native());
    }

    void Box::set_orientation(Orientation orientation)
    {
        gtk_orientable_set_orientation(GTK_ORIENTABLE(get_native()), (GtkOrientation) orientation);
    }

    Orientation Box::get_orientation() const
    {
        return (Orientation) gtk_orientable_get_orientation(GTK_ORIENTABLE(get_native()));
    }
}