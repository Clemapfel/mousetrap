//
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

#include <vector>

#include <mousetrap/box.hpp>
#include <mousetrap/log.hpp>
#include <mousetrap/orientation.hpp>

namespace mousetrap
{
    Box::Box(Orientation orientation)
        : Widget(gtk_box_new((GtkOrientation) orientation, 0)),
          CTOR_SIGNAL(Box, realize),
          CTOR_SIGNAL(Box, unrealize),
          CTOR_SIGNAL(Box, destroy),
          CTOR_SIGNAL(Box, hide),
          CTOR_SIGNAL(Box, show),
          CTOR_SIGNAL(Box, map),
          CTOR_SIGNAL(Box, unmap)
    {
        _internal = GTK_BOX(operator NativeWidget());
        g_object_ref_sink(_internal);
    }

    Box::Box(detail::BoxInternal* internal)
        : Widget(GTK_WIDGET(internal)),
          CTOR_SIGNAL(Box, realize),
          CTOR_SIGNAL(Box, unrealize),
          CTOR_SIGNAL(Box, destroy),
          CTOR_SIGNAL(Box, hide),
          CTOR_SIGNAL(Box, show),
          CTOR_SIGNAL(Box, map),
          CTOR_SIGNAL(Box, unmap)
    {
        _internal = g_object_ref(_internal);
    }
    
    Box::~Box()
    {
        g_object_unref(_internal);
    }

    NativeObject Box::get_internal() const 
    {
        return G_OBJECT(_internal);
    }

    void Box::push_back(const Widget& widget)
    {
        auto* ptr = &widget;
        WARN_IF_SELF_INSERTION(Box::push_back, this, ptr);
        gtk_box_append(GTK_BOX(operator NativeWidget()), widget.operator NativeWidget());
    }

    void Box::push_front(const Widget& widget)
    {
        auto* ptr = &widget;
        WARN_IF_SELF_INSERTION(Box::push_back, this, ptr);
        gtk_box_prepend(GTK_BOX(operator NativeWidget()), widget.operator NativeWidget());
    }

    void Box::insert_after(const Widget& to_append, const Widget& after)
    {
        auto* ptr = &to_append;
        WARN_IF_SELF_INSERTION(Box::push_back, this, ptr);
        gtk_box_insert_child_after(
            GTK_BOX(operator NativeWidget()),
            to_append.operator NativeWidget(),
            after.operator NativeWidget()
        );
    }

    void Box::remove(const Widget& widget)
    {
        gtk_box_remove(GTK_BOX(operator NativeWidget()), widget.operator GtkWidget *());
    }

    void Box::clear()
    {
        std::vector<NativeWidget> to_remove;
        NativeWidget current = gtk_widget_get_first_child(GTK_WIDGET(operator NativeWidget()));
        while (current != nullptr)
        {
            to_remove.push_back(current);
            current = gtk_widget_get_next_sibling(current);
        }

        for (auto* w : to_remove)
            gtk_box_remove(GTK_BOX(operator NativeWidget()), w);
    }

    void Box::set_homogeneous(bool b)
    {
        gtk_box_set_homogeneous(GTK_BOX(operator NativeWidget()), b);
    }

    size_t Box::get_n_items()
    {
        size_t i = 0;
        auto* child = gtk_widget_get_first_child(GTK_WIDGET(operator NativeWidget()));
        while(child != nullptr)
        {
            child = gtk_widget_get_next_sibling(child);
            i++;
        }

        return i;
    }

    bool Box::get_homogeneous() const
    {
        return gtk_box_get_homogeneous(GTK_BOX(operator NativeWidget()));
    }

    void Box::set_spacing(float spacing)
    {
        if (spacing < 0)
            log::critical("In Box::set_spacing: Spacing cannot be negative");

        gtk_box_set_spacing(GTK_BOX(operator NativeWidget()), spacing);
    }

    float Box::get_spacing() const
    {
        return gtk_box_get_spacing(GTK_BOX(operator NativeWidget()));
    }

    void Box::set_orientation(Orientation orientation)
    {
        gtk_orientable_set_orientation(GTK_ORIENTABLE(operator NativeWidget()), (GtkOrientation) orientation);
    }

    Orientation Box::get_orientation() const
    {
        return (Orientation) gtk_orientable_get_orientation(GTK_ORIENTABLE(operator NativeWidget()));
    }
}