//
// Copyright 2022 Clemens Cords
// Created on 10/26/22 by clem (mail@clemens-cords.com)
//

#include <mousetrap/center_box.hpp>
#include <mousetrap/log.hpp>

namespace mousetrap
{
    CenterBox::CenterBox(Orientation orientation)
        : Widget(gtk_center_box_new())
    {
        _internal = GTK_CENTER_BOX(operator NativeWidget());
        g_object_ref_sink(_internal);

        gtk_orientable_set_orientation(GTK_ORIENTABLE(operator NativeWidget()), (GtkOrientation) orientation);
    }
    
    CenterBox::CenterBox(detail::CenterBoxInternal* internal) 
        : Widget(GTK_WIDGET(internal))
    {
        _internal = g_object_ref(internal);
    }
    
    CenterBox::~CenterBox()
    {
        g_object_unref(_internal);
    }

    NativeObject CenterBox::get_internal() const
    {
        return G_OBJECT(_internal);
    }

    void CenterBox::set_start_child(const Widget& widget)
    {
        auto* ptr = &widget;
        WARN_IF_SELF_INSERTION(CenterBox::set_start_widget, this, ptr);

        gtk_center_box_set_start_widget(GTK_CENTER_BOX(operator NativeWidget()), widget.operator NativeWidget());
    }

    void CenterBox::remove_start_widget()
    {
        gtk_center_box_set_start_widget(GTK_CENTER_BOX(operator NativeWidget()), nullptr);
    }

    void CenterBox::set_end_child(const Widget& widget)
    {
        auto* ptr = &widget;
        WARN_IF_SELF_INSERTION(CenterBox::set_end_widget, this, ptr);

        gtk_center_box_set_end_widget(GTK_CENTER_BOX(operator NativeWidget()), widget.operator NativeWidget());
    }

    void CenterBox::remove_end_widget()
    {
        gtk_center_box_set_end_widget(GTK_CENTER_BOX(operator NativeWidget()), nullptr);
    }

    void CenterBox::set_center_child(const Widget& widget)
    {
        auto* ptr = &widget;
        WARN_IF_SELF_INSERTION(CenterBox::set_center_widget, this, ptr);

        gtk_center_box_set_center_widget(GTK_CENTER_BOX(operator NativeWidget()), widget.operator NativeWidget());
    }

    void CenterBox::remove_center_widget()
    {
        gtk_center_box_set_center_widget(GTK_CENTER_BOX(operator NativeWidget()), nullptr);
    }

    void CenterBox::set_orientation(Orientation orientation)
    {
        gtk_orientable_set_orientation(GTK_ORIENTABLE(operator NativeWidget()), (GtkOrientation) orientation);
    }

    Orientation CenterBox::get_orientation() const
    {
        return (Orientation) gtk_orientable_get_orientation(GTK_ORIENTABLE(operator NativeWidget()));
    }
}