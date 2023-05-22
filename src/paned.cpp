//
// Copyright 2022 Clemens Cords
// Created on 9/18/22 by clem (mail@clemens-cords.com)
//

#include <mousetrap/paned.hpp>
#include <mousetrap/log.hpp>

namespace mousetrap
{
    Paned::Paned(Orientation orientation)
        : Widget(gtk_paned_new((GtkOrientation) orientation))
    {
        _internal = g_object_ref_sink(GTK_PANED(Widget::operator NativeWidget()));

        set_start_child_resizable(true);
        set_end_child_resizable(true);
        set_has_wide_handle(true);
    }
    
    Paned::Paned(detail::PanedInternal* internal) 
        : Widget(GTK_WIDGET(internal))
    {
        _internal = g_object_ref(internal);
    }

    Paned::~Paned()
    {
        g_object_unref(_internal);
    }

    NativeObject Paned::get_internal() const
    {
        return G_OBJECT(_internal);
    }

    void Paned::set_position(int v)
    {
        gtk_paned_set_position(GTK_PANED(operator NativeWidget()), v);
    }

    int Paned::get_position()
    {
        return gtk_paned_get_position(GTK_PANED(operator NativeWidget()));
    }

    void Paned::set_start_child(const Widget& widget)
    {
        auto* ptr = &widget;
        WARN_IF_SELF_INSERTION(Paned::set_start_child, this, ptr);
        gtk_paned_set_start_child(GTK_PANED(operator NativeWidget()), widget.operator NativeWidget());
    }

    void Paned::set_start_child_resizable(bool b)
    {
        gtk_paned_set_resize_start_child(GTK_PANED(operator NativeWidget()), b);
    }

    bool Paned::get_start_child_resizable() const
    {
        return gtk_paned_get_resize_start_child(GTK_PANED(operator NativeWidget()));
    }

    void Paned::set_start_child_shrinkable(bool b)
    {
        gtk_paned_set_shrink_start_child(GTK_PANED(operator NativeWidget()), b);
    }

    bool Paned::get_start_child_shrinkable() const
    {
        return gtk_paned_get_shrink_start_child(GTK_PANED(operator NativeWidget()));
    }

    void Paned::set_end_child(const Widget& widget)
    {
        auto* ptr = &widget;
        WARN_IF_SELF_INSERTION(Paned::set_end_child, this, ptr);
        gtk_paned_set_end_child(GTK_PANED(operator NativeWidget()), widget.operator NativeWidget());
    }

    void Paned::set_end_child_resizable(bool b)
    {
        gtk_paned_set_resize_end_child(GTK_PANED(operator NativeWidget()), b);
    }

    bool Paned::get_end_child_resizable() const
    {
        return gtk_paned_get_resize_end_child(GTK_PANED(operator NativeWidget()));
    }

    void Paned::set_end_child_shrinkable(bool b)
    {
        gtk_paned_set_shrink_end_child(GTK_PANED(operator NativeWidget()), b);
    }

    bool Paned::get_end_child_shrinkable() const
    {
        return gtk_paned_get_shrink_end_child(GTK_PANED(operator NativeWidget()));
    }

    void Paned::set_has_wide_handle(bool b)
    {
        gtk_paned_set_wide_handle(GTK_PANED(operator NativeWidget()), b);
    }

    bool Paned::get_has_widget_handle() const
    {
        return gtk_paned_get_wide_handle(GTK_PANED(operator NativeWidget()));
    }

    void Paned::set_orientation(Orientation orientation)
    {
        gtk_orientable_set_orientation(GTK_ORIENTABLE(GTK_PANED(operator NativeWidget())), (GtkOrientation) orientation);
    }

    Orientation Paned::get_orientation() const
    {
        return (Orientation) gtk_orientable_get_orientation(GTK_ORIENTABLE(GTK_PANED(operator NativeWidget())));
    }
}