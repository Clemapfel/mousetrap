//
// Created by clem on 8/18/23.
//

#include <mousetrap/flow_box.hpp>

namespace mousetrap
{
    FlowBox::FlowBox(Orientation orientation)
    : Widget(gtk_flow_box_new()),
      CTOR_SIGNAL(FlowBox, realize),
      CTOR_SIGNAL(FlowBox, unrealize),
      CTOR_SIGNAL(FlowBox, destroy),
      CTOR_SIGNAL(FlowBox, hide),
      CTOR_SIGNAL(FlowBox, show),
      CTOR_SIGNAL(FlowBox, map),
      CTOR_SIGNAL(FlowBox, unmap)
    {
        _internal = GTK_FLOW_BOX(operator NativeWidget());
        g_object_ref_sink(_internal);

        set_orientation(orientation);
    }

    FlowBox::FlowBox(detail::FlowBoxInternal* internal)
    : Widget(GTK_WIDGET(internal)),
      CTOR_SIGNAL(FlowBox, realize),
      CTOR_SIGNAL(FlowBox, unrealize),
      CTOR_SIGNAL(FlowBox, destroy),
      CTOR_SIGNAL(FlowBox, hide),
      CTOR_SIGNAL(FlowBox, show),
      CTOR_SIGNAL(FlowBox, map),
      CTOR_SIGNAL(FlowBox, unmap)
    {
        _internal = g_object_ref(_internal);
    }

    FlowBox::~FlowBox()
    {
        g_object_unref(_internal);
    }

    NativeObject FlowBox::get_internal() const
    {
        return G_OBJECT(_internal);
    }

    void FlowBox::push_back(const Widget& widget)
    {
        auto* ptr = &widget;
        WARN_IF_SELF_INSERTION(FlowBox::push_back, this, ptr);
        WARN_IF_PARENT_EXISTS(FlowBox::push_back, widget);
        gtk_flow_box_append(GTK_FLOW_BOX(operator NativeWidget()), widget.operator NativeWidget());
    }

    void FlowBox::push_front(const Widget& widget)
    {
        auto* ptr = &widget;
        WARN_IF_SELF_INSERTION(FlowBox::push_front, this, ptr);
        WARN_IF_PARENT_EXISTS(FlowBox::push_front, widget);
        gtk_flow_box_prepend(GTK_FLOW_BOX(operator NativeWidget()), widget.operator NativeWidget());
    }

    void FlowBox::insert(const Widget& to_append, uint64_t index)
    {
        auto* ptr = &to_append;
        WARN_IF_SELF_INSERTION(FlowBox::insert_after, this, ptr);
        WARN_IF_PARENT_EXISTS(FlowBox::insert_after, to_append);
        gtk_flow_box_insert(GTK_FLOW_BOX(operator NativeWidget()), to_append.operator NativeWidget(), index);
    }

    void FlowBox::remove(const Widget& widget)
    {
        gtk_flow_box_remove(GTK_FLOW_BOX(operator NativeWidget()), widget.operator GtkWidget *());
    }

    void FlowBox::clear()
    {
        std::vector<NativeWidget> to_remove;
        NativeWidget current = gtk_widget_get_first_child(GTK_WIDGET(operator NativeWidget()));
        while (current != nullptr)
        {
            to_remove.push_back(current);
            current = gtk_widget_get_next_sibling(current);
        }

        for (auto* w : to_remove)
            gtk_flow_box_remove(GTK_FLOW_BOX(operator NativeWidget()), w);
    }

    void FlowBox::set_homogeneous(bool b)
    {
        gtk_flow_box_set_homogeneous(GTK_FLOW_BOX(operator NativeWidget()), b);
    }

    uint64_t FlowBox::get_n_items()
    {
        uint64_t i = 0;
        auto* child = gtk_widget_get_first_child(GTK_WIDGET(operator NativeWidget()));
        while(child != nullptr)
        {
            child = gtk_widget_get_next_sibling(child);
            i++;
        }

        return i;
    }

    bool FlowBox::get_homogeneous() const
    {
        return gtk_flow_box_get_homogeneous(GTK_FLOW_BOX(operator NativeWidget()));
    }

    void FlowBox::set_row_spacing(float spacing)
    {
        if (spacing < 0)
            log::critical("In FlowBox::set_spacing: Spacing cannot be negative");

        gtk_flow_box_set_row_spacing(GTK_FLOW_BOX(operator NativeWidget()), spacing);
    }

    float FlowBox::get_row_spacing() const
    {
        return gtk_flow_box_get_row_spacing(GTK_FLOW_BOX(operator NativeWidget()));
    }

    void FlowBox::set_column_spacing(float spacing)
    {
        if (spacing < 0)
            log::critical("In FlowBox::set_spacing: Spacing cannot be negative");

        gtk_flow_box_set_column_spacing(GTK_FLOW_BOX(operator NativeWidget()), spacing);
    }

    float FlowBox::get_column_spacing() const
    {
        return gtk_flow_box_get_column_spacing(GTK_FLOW_BOX(operator NativeWidget()));
    }

    void FlowBox::set_orientation(Orientation orientation)
    {
        gtk_orientable_set_orientation(GTK_ORIENTABLE(operator NativeWidget()), (GtkOrientation) orientation);
    }

    Orientation FlowBox::get_orientation() const
    {
        return (Orientation) gtk_orientable_get_orientation(GTK_ORIENTABLE(operator NativeWidget()));
    }
}